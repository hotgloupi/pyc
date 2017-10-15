import contextlib

from ..parser import Token
from .. import ast as parser_ast
from . import ast
from . import ts
from .scope import Scope
from . import internal

__all__ = ['convert']

class Context:

    def __init__(self, scope):
        self.scope = scope
        self.return_statements = []

    @property
    def return_type(self):
        if not self.return_statements:
            return ts.Type('void')
        assert len(self.return_statements) == 1
        return self.return_statements[0].type

class Converter(parser_ast.Visitor):

    def __init__(self, module, builtins):
        self.module = module
        self.context = None
        self.indent = 0
        self.builtins = builtins
        if builtins is not None:
            self.scope = Scope(builtins.core_ast.scope, '#root')
        else:
            self.scope = Scope(None, name = '#root')
        self.scope['__pyc__'] = internal.intrinsics

    def visit(self, node):
        self.indent += 1
        res = super().visit(node)
        self.indent -= 1
        return res

    def _visit_all(self, elements):
        return list(filter(None, map(self.visit, elements)))

    def debug(self, *args, **kw):
        print('#' * self.indent, *args, **kw)

    @contextlib.contextmanager
    def new_scope(self, name = None):
        self.scope = Scope(parent = self.scope, name = name)
        yield self.scope
        assert self.scope is not None
        self.scope = self.scope.parent

    def visit_Block(self, node):
        with self.new_scope() as scope:
            return ast.Block(
                node.loc,
                statements = list(filter(None, map(self.visit, node.statements))),
                scope = scope,
            )

    def visit_FunctionDefinition(self, node):
        self.scope[node.name] = ast.FunctionTemplate(
            node.loc,
            definition = node,
            specializations = {},
            scope = self.scope
        )
        return None

    def visit_Identifier(self, node):
        return ast.Variable(
            node.loc,
            id = node.name,
            ref = self.scope[node.name]
        )

    def visit_ReturnStatement(self, node):
        ret = ast.Return(node.loc, value = self.visit(node.expression))
        self.context.return_statements.append(ret)
        return ret

    def visit_ExpressionList(self, node):
        return self._visit_all(node.expressions)

    def visit_ExpressionStatement(self, node):
        return self.visit(node.expression)

    def _resolve_function_template(self, expr):
        assert isinstance(expr, parser_ast.Identifier)
        fn = self.scope.find(expr.name)
        if fn is None:
            raise Exception("Cannot find %s" % expr.name)
        if not isinstance(fn, ast.FunctionTemplate):
            raise Exception("'%s' is not a function" % expr.name)
        return fn

    @contextlib.contextmanager
    def swap_context(self):
        old_context = self.context
        self.context = Context(self.scope)
        yield self.context
        self.context = old_context

    def _visit_function_block(self, body):
        with self.swap_context() as ctx:
            body = self.visit(body)
            return self.context.return_type, body

    @contextlib.contextmanager
    def swap_scope(self, new_scope):
        old_scope = self.scope
        self.scope = new_scope
        yield
        self.scope = old_scope

    def _specialize(self, fn, args):
        with self.swap_scope(fn.scope):
            with self.new_scope('%s#args' % fn.definition.name) as scope:
                for arg in args:
                    scope[arg.id] = arg
                return_type, body = self._visit_function_block(fn.definition.body)

        return ast.Function(
            fn.loc,
            name = fn.definition.name,
            return_type = return_type,
            args = args,
            body = body,
        )

    def _args(self, fn, args):
        map_args = {}
        for i, arg in enumerate(args):
            map_args[arg.name or i] = arg
        res = []
        for i, param in enumerate(fn.parameters):
            arg = map_args.get(param.name)
            if arg is None:
                arg = map_args.get(i)
            if arg is None:
                # XXX use param.default if available
                raise Exception("Missing parameter '%s'" % param)
            res.append(
                ast.Variable(arg.loc, param.name, self.visit(arg.value))
            )
        return res

    def visit_FunctionCall(self, node):
        template = self._resolve_function_template(node.expression)
        args = self._args(template.definition, node.arguments)
        arg_values = tuple(arg.ref for arg in args)
        arg_types = tuple(arg.type for arg in args)
        # XXX check if specialization exists first
        fn = \
            template.specializations[tuple(arg_types)] = \
            self._specialize(template, args)
        return ast.FunctionCall(
            node.loc,
            fn = fn,
            args = arg_values,
        )

    def _cast(self, var, type):
        assert isinstance(var, ast.Variable)
        assert isinstance(type, ts.Type)
        print("CAST", var, "FROM", var.type, "TO", type)
        return var.ref

    def visit_ExternFunctionCall(self, node):
        args = []
        for arg, type in zip(node.arguments, node.signature[1:]):
            var = self.scope[arg]
            args.append(
                ast.Variable(var.loc, '#' + var.id, self._cast(var, type))
            )
        return ast.FunctionCall(
            node.loc,
            fn = ast.Function(
                loc = node.loc,
                name = node.name,
                return_type = node.signature[0],
                args = args,
                body = None,
            ),
            args = [arg.ref for arg in args]
        )

    def visit_BinaryExpression(self, node):
        return ast.PrimaryOperator(
            node.loc,
            op = node.op,
            args = self._visit_all([node.lhs, node.rhs])
        )

    def visit_Number(self, node):
        return ast.LiteralInteger(node.loc, int(node.value))

    def visit_children(self, node: ast.Node):
        raise NotImplementedError(
            'no method visit_' + node.__class__.__name__ + '()'
        )

    def visit_ImportStatement(self, node):
        if node.style == parser_ast.ImportStatement.Style.absolute:
            module = self.module.manager.load_absolute(
                node.from_,
                self.builtins
            )
            import_ast = module.core_ast
            for import_ in node.imports:
                assert len(import_.dotted_name) == 1
                name = import_.dotted_name[0]
                self.scope[name] = import_ast.scope[name]
            return ast.FunctionCall(
                node.loc,
                fn = import_ast.entry,
                args = [],
            )

    def visit_PassStatement(self, node):
        pass

    def visit_Assignment(self, node):
        assert node.op == Token.EQUAL
        assert isinstance(node.lhs, parser_ast.Identifier)
        self.scope[node.lhs.name] = node.rhs

    def visit_GetAttr(self, node):
        assert isinstance(node.lhs, parser_ast.Identifier)
        assert isinstance(node.rhs, parser_ast.Identifier)
        lhs = self.scope[node.lhs.name]
        return lhs.scope[node.rhs.name]

    def _make_extern_template(self, node):
        return_type = self.visit(node.definition.annotation)
        signature = [return_type] + [
            self.visit(parameter.annotation)
            for parameter in node.definition.parameters
        ]

        body = [
            parser_ast.ReturnStatement(
                loc = node.loc,
                expression = internal.ExternFunctionCall(
                    loc = node.loc,
                    name = node.definition.name,
                    arguments = [
                        p.name for p in node.definition.parameters
                    ],
                    signature = [ts.Type(ctype) for ctype in signature],
                )
            )
        ]

        definition = parser_ast.FunctionDefinition(
            loc = node.definition.loc,
            name = node.definition.name,
            parameters = node.definition.parameters,
            body = parser_ast.Block(
                loc = node.definition.loc,
                statements = body
            ),
            annotation = return_type,
        )
        return ast.FunctionTemplate(
            node.loc,
            definition = definition,
            specializations = {},
            scope = Scope(parent = None)
        )

    def visit_DecoratedDefinition(self, node):
        decorator = self.visit(node.decorator)
        print("Decorator", decorator)
        if isinstance(decorator, internal.ExternDecorator):
            extern = self._make_extern_template(node)
            self.scope[node.definition.name] = extern
            return None
        assert False

        return self.visit(node.definition)

def convert(module, py_ast: ast.Node, builtins):
    if module.name == '__main__':
        fname = 'main'
    else:
        fname = '#init_' + module.name
    return_type = None
    body = Converter(module, builtins).visit(py_ast)
    fn = ast.Function(
        body.loc,
        name = fname,
        return_type = ts.Type('void'),
        args = [],
        body = body,
    )
    return ast.ModuleEntry(
        body.loc,
        module = module,
        entry = fn,
        scope = body.scope,
    )
