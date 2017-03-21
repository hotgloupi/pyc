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

    def find(self, key):
        value = self.values.get(key)
        if value is not None:
            return value
        if self.parent is not None:
            return self.parent.find(key)
        return None

    def __repr__(self):
        if self.parent is not None:
            return str(self.parent) + '.' + self.name
        return '@' + self.name

class Converter(parser_ast.Visitor):

    def __init__(self):
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

    def push_scope(self):
        self.scope = Scope(parent = self.scope)
        return self.scope

    def pop_scope(self):
        assert self.scope is not None
        self.scope = self.scope.parent

    def visit_Block(self, node):
        scope = self.push_scope()
        res = ast.Block(
            node.loc,
            statements = list(filter(None, map(self.visit, node.statements))),
            scope = scope,
        )
        self.pop_scope()
        return res

    def visit_FunctionDefinition(self, node):
        self.scope[node.name] = ast.FunctionTemplate(
            node.loc,
            definition = node,
            specializations = {},
            scope = self.scope
        )
        return None

    def visit_Identifier(self, node):
        return ast.Variable(node.loc, id = node.name)

    def visit_ReturnStatement(self, node):
        return ast.Return(node.loc, value = self.visit(node.expression))

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

    def _visit_function_block(self, body):
        body = self.visit(body)
        return None, body

    @contextlib.contextmanager
    def swap_scope(self, new_scope):
        old_scope = self.scope
        self.scope = new_scope
        yield
        self.scope = old_scope

    def _specialize(self, fn, args):
        with self.swap_scope(fn.scope):
            ret, body = self._visit_function_block(fn.definition.body)

        return ast.Function(
            fn.loc,
            name = fn.definition.name,
            signature = [ret, args],
            body = body,
            template = fn,
        )


    def visit_FunctionCall(self, node):
        template = self._resolve_function_template(node.expression)
        args = self.visit(node.arguments)
        fn = template.specializations[tuple(args)] = self._specialize(template, args)
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
