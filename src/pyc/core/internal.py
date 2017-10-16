
from . import ast
from .. import ast as parser_ast
from .scope import Scope

scope = Scope(parent = None, name = '__pyc__')

class ExternFunctionCall(parser_ast.Node):
    fields = ('name', 'arguments', 'signature')

class ExternDecorator(ast.Node):
    fields = tuple()

class CastExpression(parser_ast.Node):
    fields = ('name', 'to')

scope['extern'] = ExternDecorator(None)

types = [
    'ssize_t',
    'size_t',
    'void_p',
    'int',
]
for type in types:
    scope[type] = type

intrinsics = ast.ModuleEntry(
    loc = None,
    module = None,
    entry = None,
    scope = scope,
)
