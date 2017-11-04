from .. import ast as parser_ast

from . import ts

class Node(parser_ast.Node):
    kind = 'core'

    __type = None
    @property
    def type(self):
        if self.__type is None:
            self.__type = self.make_type()
        return self.__type

    @type.setter
    def type(self, value):
        assert value is not None
        self.__type = value

    def make_type(self):
        raise NotImplementedError("Cannot find type for %s" % self)

class ModuleEntry(Node):
    fields = ('module', 'entry', 'scope')

    def make_type(self):
        return ts.Type('void')

class Block(Node):
    fields = ('statements', 'scope')

    def make_type(self):
        return ts.Type('void')

class Variable(Node):
    fields = ('id', 'ref')

    @property
    def type(self):
        return self.ref.type


class Return(Node):
    fields = ('value', )

    @property
    def type(self):
        return self.value.type

class Cast(Node):
    fields = ('value', 'to_type')

    @property
    def type(self):
        return self.to_type

#class Assign(Node):
#    fields = ("ref", "val", "type")
#
#    def __init__(self, ref, val, type=None):
#        self.ref = ref
#        self.val = val
#        self.type = type

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

    def make_type(self):
        return ts.Type('int')

class LiteralString(Node):
    fields = ('value', )

    def make_type(self):
        return ts.Type('char_p')

class Function(Node):
    fields = ('name', 'parameters', 'return_type', 'body')

    def make_type(self):
        return ts.App([param.type for param in self.parameters], self.return_type)

class Parameter(Node):
    fields = ('name', 'type')

class FunctionCall(Node):
    fields = ('fn', 'args')

    def make_type(self):
        return self.fn.return_type

class PrimaryOperator(Node):
    fields = ('op', 'args')

    @property
    def type(self):
        assert len(set(arg.type for arg in self.args)) == 1
        return self.args[0].type

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
