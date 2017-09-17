import contextlib

from .. import ast as parser_ast
from . import ast


__all__ = ['convert']

class Scope():
    __index = [1]

    def __init__(self, parent, name = None):
        self.parent = parent
        self.values = {}
        if name is None:
            self.name = str(self.__index[0])
            self.__index[0] += 1
        else:
            self.name = name

    def __setitem__(self, key, value):
        self.values[key] = value

    def __getitem__(self, key):
        value = self.find(key)
        if value is None:
            raise Exception("Couln't find '%s'" % key)
        return value

    def find(self, key):
        value = self.values.get(key)
        if value is not None:
            return value
        if self.parent is not None:
            return self.parent.find(key)
        return None

    @property
    def path(self):
        if self.parent is not None:
            return self.parent.path + '.' + self.name
        return self.name

    def __repr__(self):
        return '@' + self.path

class Context:

    def __init__(self, scope):
        self.scope = scope
        self.return_statements = []

    @property
    def return_type(self):
        if not self.return_statements:
            return None
        assert len(self.return_statements) == 1
        return self.return_statements[0].type

class Converter(parser_ast.Visitor):

    def __init__(self):
        self.context = None
        self.scope = None
        self.indent = 0

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
                for identifier, arg in zip(fn.definition.args.expressions, args):
                    scope[identifier.name] = arg
                return_type, body = self._visit_function_block(fn.definition.body)

        arguments = []
        for identifier, arg in zip(fn.definition.args.expressions, args):
            arguments.append(ast.Variable(arg.loc, id = identifier.name, ref = arg))
        return ast.Function(
            fn.loc,
            name = fn.definition.name,
            return_type = return_type,
            args = arguments,
            body = body,
        )

    def visit_FunctionCall(self, node):
        template = self._resolve_function_template(node.expression)
        args = self.visit(node.arguments)
        fn = \
            template.specializations[tuple(args)] = \
            self._specialize(template, args)
        return ast.FunctionCall(
            node.loc,
            fn = fn,
            args = args,
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

def convert(py_ast: ast.Node):
    return Converter().visit(py_ast)
