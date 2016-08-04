class Node(): pass
class AssertStatement(Node): pass
class Assignment(Node): pass
class AsyncDefinition(Node): pass
class BinaryExpression(Node): pass
class Block(Node): pass
class BreakStatement(Node): pass
class ClassDefinition(Node): pass
class ContinueStatement(Node): pass
class DecoratedDefinition(Node): pass
class DelStatement(Node): pass
class Expression(Node): pass

class ExpressionList(Node):
    def __init__(self):
        self.values = []

class ExpressionStatement(Node): pass
class ForStatement(Node): pass

class FunctionCall(Node):
    def __init__(self, expr, args):
        self.expr = expr
        self.args = args

class FunctionDefinition(Node):
    def __init__(self, name, args, body):
        self.name = name
        self.args = args
        self.body = body

class GetItem(Node): pass
class GlobalStatement(Node): pass

class Identifier(Node):
    def __init__(self, str):
        self.value = str

class IfStatement(Node): pass
class ImportStatement(Node): pass

class NamedArgument(Node):
    def __init__(self, name, value):
        self.name = name
        self.value = value

class NodeKind(Node): pass
class NonLocalStatement(Node): pass

class Number(Node):
    def __init__(self, str):
        self.value = str

class PassStatement(Node): pass
class RaiseStatement(Node): pass
class ReturnStatement(Node): pass
class Slice(Node): pass
class Statement(Node): pass
class String(Node): pass
class TryStatement(Node): pass
class UnaryExpression(Node): pass
class WhileStatement(Node): pass
class WithStatement(Node): pass
class YieldStatement(Node): pass
