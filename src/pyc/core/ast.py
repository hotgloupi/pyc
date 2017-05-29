from .. import ast as parser_ast

class Node(parser_ast.Node):
    kind = 'core'
    pass

class Block(Node):
    fields = ('statements', 'scope')

class Variable(Node):
    fields = ('id', )


class Return(Node):
    fields = ('value', )


#class Var(Node):
#    fields = ("id", "type")
#
#    def __init__(self, id, type=None):
#        self.id = id
#        self.type = type
#
#class Assign(Node):
#    fields = ("ref", "val", "type")
#
#    def __init__(self, ref, val, type=None):
#        self.ref = ref
#        self.val = val
#        self.type = type
#
#class Return(Node):
#    fields = ("val")
#
#    def __init__(self, val):
#        self.val = val
#
#class Loop(Node):
#    fields = ("var", "begin", "end", "body")
#
#    def __init__(self, var, begin, end, body):
#        self.var = var
#        self.begin = begin
#        self.end = end
#        self.body = body
#
#class App(Node):
#    fields = ("fn", "args")
#
#    def __init__(self, fn, args):
#        self.fn = fn
#        self.args = args
#

class FunctionTemplate(Node):
    fields = ('definition', 'specializations', 'scope')

class LiteralInteger(Node):
    fields = ('value',)

class Function(Node):
    fields = ('name', 'signature', 'body')

class FunctionCall(Node):
    fields = ('fn', 'args')

class PrimaryOperator(Node):
    fields = ('op', 'args')

#
#class LitInt(Node):
#    fields = ("n")
#
#    def __init__(self, n, type=None):
#        self.n = n
#        self.type = type
#
#class LitFloat(Node):
#    fields = ("n")
#
#    def __init__(self, n, type=None):
#        self.n = n
#        self.type = None
#
#class LitBool(Node):
#    fields = ("n")
#
#    def __init__(self, n):
#        self.n = n
#
#class Prim(Node):
#    fields = ("fn", "args")
#
#    def __init__(self, fn, args):
#        self.fn = fn
#        self.args = args
#
#class Index(Node):
#    fields = ("val", "ix")
#
#    def __init__(self, val, ix):
#        self.val = val
#        self.ix = ix
#
#class Noop(Node):
#    fields = ()
#
