from .. import ast as parser_ast
from . import ast

__all__ = ['convert']

class Scope():
    def __init__(self, parent):
        self.parent = parent
        self.values = {}

    def __setitem__(self, key, value):
        self.values[key] = value

    def find(self, key):
        value = self.values.get(key)
        if value is not None:
            return value
        if self.parent is not None:
            return self.parent.find(key)
        return None

class Converter(parser_ast.Visitor):

    def __init__(self):
        self.scopes = []
        self.indent = 0

    @property
    def scope(self):
        return self.scopes[-1]

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
        if self.scopes:
            s = Scope(parent = self.scope)
        else:
            s = Scope(parent = None)
        self.scopes.append(s)
        return s

    def pop_scope(self):
        self.scopes.pop()

    def visit_Block(self, node):
        scope = self.push_scope()
        self.debug("START BLOCK")
        res = ast.Block(
            node.loc,
            statements = list(filter(None, map(self.visit, node.statements))),
            scope = scope,
        )
        self.debug("END BLOCK")
        self.pop_scope()
        return res

    def visit_FunctionDefinition(self, node):
        self.debug("FUNC", node.name)
        res = self.scope[node.name] = ast.FunctionTemplate(
            node.loc,
            definition = node,
            specializations = {},
        )
        return res

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

    def visit_FunctionCall(self, node):
        fn = self._resolve_function_template(node.expression)
        args = self.visit(node.arguments)
        fn.specializations[tuple(args)] = None
        print('CALLING', fn, args)

    def visit_Number(self, node):
        return ast.LiteralInteger(node.loc, int(node.value))

    def visit_children(self, node: ast.Node):
        raise NotImplementedError(
            'no method visit_' + node.__class__.__name__ + '()'
        )

def convert(py_ast: ast.Node):
    return Converter().visit(py_ast)
