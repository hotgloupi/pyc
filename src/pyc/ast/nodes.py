from enum import Enum

class NodeCreator(type):

    def __new__(cls, name, bases, attrs):
        if name != 'Node':
            if attrs.get('fields') is None:
                if any(hasattr(b, 'fields') for b in bases):
                    pass  # Let's use parents constructor
                else:
                    raise Exception("You must specify a fields attribute in class '%s'" % name)
            else:
                attrs['__init__'] = cls.make_ctor(attrs['fields'], bases)
        return super().__new__(cls, name, bases, attrs)

    #def __call__(cls, *args, **kw):
    #    print('##', cls, args, kw)
    #    return super().__call__(*args, **kw)

    @classmethod
    def make_ctor(cls, fields, bases):
        def init(self, loc, *args, **kw):
            if self.__class__ == Node:
                return

            values = list(args)
            if len(fields) > len(args):
                for field in fields[len(args):]:
                    if field not in kw:
                        raise Exception("Missing argument '%s'" % field)
                    values.append(kw.pop(field))
                if len(kw):
                    raise Exception("Unexpected arguments %s" % kw)
            elif len(fields) < len(args):
                raise Exception(
                    "Too many arguments, expected %s, got %s" % (
                        len(fields), len(args)
                    )
                )
            self.loc = loc
            for k, v in zip(fields, values):
                setattr(self, k, v)
        init.__name__ = '__init__'
        return init


class Node(metaclass = NodeCreator):
    kind = 'ast'

    def __str__(self):
        return "<%s(%s) at %s>" % (
            self.__class__.__name__,
            ', '.join('%s=%s' % (f, getattr(self, f)) for f in self.fields),
            self.loc
        )


class AssertStatement(Node):
    fields = ('expressions',)

class Assignment(Node):
    fields = ('op', 'lhs', 'rhs',)

class AsyncDefinition(Node):
    fields = ('definition',)

class BinaryExpression(Node):
    fields = ('op', 'lhs', 'rhs', )

class BreakStatement(Node):
    fields = ()

class ClassDefinition(Node):
    fields = ('name', 'bases', 'body')

class ContinueStatement(Node):
    fields = ()

class DecoratedDefinition(Node):
    fields = ('decorator', 'definition')

class DelStatement(Node):
    fields = ('expression', )

class Expression(Node):
    fields = ('value',)

class Block(Node):
    fields = ('statements', )

class ExpressionList(Node):
    fields = ('expressions', )

class ExpressionStatement(Node):
    fields = ('expression', )

class ForStatement(Node):
    fields = ()

class FunctionCall(Node):
    fields = ('expression', 'arguments')

class FunctionDefinition(Node):
    fields = ('name', 'args', 'body', )

class GetItem(Node):
    fields = () #XXX

class GlobalStatement(Node):
    fields = () #XXX

class Identifier(Node):
    fields = ('name', )

class IfStatement(Node):
    fields = () #XXX

class ImportStatement(Node):
    class Style(Enum):
        absolute = 1
        relative_to_parent = 2
        relative_to_current = 3

    class Import:
        dotted_name = None
        rename = None

    fields = ('style', 'from_', 'imports')

class NamedArgument(Node):
    fields = ('name', 'value', )

class NodeKind(Node):
    fields = () #XXX

class NonLocalStatement(Node):
    fields = () #XXX

class Number(Node):
    fields = ('value', )

class PassStatement(Node):
    fields = () #XXX

class RaiseStatement(Node):
    fields = () #XXX

class ReturnStatement(Node):
    fields = ('expression', )

class Slice(Node):
    fields = () #XXX

class Statement(Node):
    fields = () #XXX

class String(Node):
    fields = ('value', )

class TryStatement(Node):
    fields = () #XXX

class UnaryExpression(Node):
    fields = () #XXX

class WhileStatement(Node):
    fields = () #XXX

class WithStatement(Node):
    fields = () #XXX

class YieldStatement(Node):
    fields = () #XXX

