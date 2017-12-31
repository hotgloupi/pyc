import contextlib
import sys

from ..ast import Visitor

__all__ = ['dump']

class Dump(Visitor):

    def __init__(self, file):
        self.out = file
        self.indent = -1
        self.indent_width = 4
        self.seen_functions = set()
        self.should_indent_next = True

    @contextlib.contextmanager
    def _indent(self):
        self.indent += 1
        yield
        self.indent -= 1

    def write(self, *args, **kw):
        kw['file'] = self.out
        kw.setdefault('end', '')
        pad = ''
        if self.should_indent_next:
            pad = ' ' * max((self.indent * self.indent_width - 1), 0)
            self.should_indent_next = False
        print(pad, *args, **kw)

    def writeln(self, *args, **kw):
        kw['end'] = '\n'
        self.write(*args, kw)
        self.should_indent_next = True

    def visit(self, node):
        print("-->", node, getattr(node, 'fields', '(no fields)'))
        super().visit(node)

    def visit_Block(self, node):
        self.write('//', node.scope)
        for st in node.statements:
            self.visit(st)
            self.writeln()

    def visit_Function(self, node):
        def pretty_param(p):
            return "%s: %s" % (p.name, p.type.name)
        params = ', '.join(map(pretty_param, node.parameters))
        self.writeln(
            'fn', '%s(%s)' % (node.name, params),
            '->', node.return_type.name, '{'
        )
        with self._indent():
            self.visit(node.body)
        self.writeln('}')

    def visit_FunctionCall(self, node):
        if node.fn not in self.seen_functions:
            self.visit(node.fn)
            self.seen_functions.add(node.fn)
        def pretty_arg(arg): return repr(arg.value)
        args = ', '.join(map(pretty_arg, node.args))
        self.write('%s(%s)' % (node.fn.name, args))

    def visit_Return(self, node):
        self.write('return ')
        self.visit(node.value)
        self.writeln()

    def visit_ModuleEntry(self, node):
        self.visit(node.entry)

def dump(node, file = sys.stdout):
    Dump(file).visit(node)
