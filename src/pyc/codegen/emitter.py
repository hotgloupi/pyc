from .. import llvm

import contextlib
from collections import namedtuple

__all__ = [
    'Emitter',
]

class Emitter:
    def __init__(self, module_name):
        self.ctx = llvm.Context()
        self.module = llvm.Module(module_name, self.ctx)
        self.type_factory = llvm.TypeFactory(self.ctx)
        main_sig = self.type_factory.function(self.type_factory.int32(), ())
        self.main = self.module.add_function('main', main_sig)
        self.main_entry = self.main.add_block("entry")
        #self.main_exit = self.main.add_block("exit")
        self.stack = [self.main]
        self.functions = {}

    @property
    def fn(self):
        return self.stack[-1]

    @contextlib.contextmanager
    def push_fn(self, fn):
        self.stack.append(fn)
        yield
        self.stack.pop()

    @property
    def builder(self):
        return self.fn.last_block.builder

    def visit_Block(self, node):
        self.fn.add_block(node.scope.path)
        for stmt in node.statements:
            self.visit(stmt)

    def _find_function(self, function_node):
        if function_node not in self.functions:
            self.visit(function_node)
        return self.functions[function_node]

    def _make_value(self, node):
        pass

    def visit_FunctionCall(self, node):
        fn = self._find_function(node.fn)
        args = [self._make_value(arg) for arg in node.args]
        self.builder.call(fn, ())#node.args)

    def visit_Function(self, node):
        assert node not in self.functions
        sig = self.type_factory.function(self.type_factory.void(), ())
        fn = self.module.add_function(node.name, sig)
        fn.add_block('entry')
        self.functions[node] = fn
        with self.push_fn(fn):
            self.visit(node.body)
        return fn

    def visit_Return(self, node):
        pass

    def visit(self, node):
        cls = node.__class__.__name__
        method = getattr(self, 'visit_' + cls)
        return method(node)
