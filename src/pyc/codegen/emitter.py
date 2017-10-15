from .. import llvm

from .. import core

import contextlib
from collections import namedtuple

__all__ = [
    'Emitter',
]

class FunctionContext:
    def __init__(self, fn, builder, entry_block, exit_block):
        self.fn = fn
        self.builder = builder
        self.entry_block = entry_block
        self.exit_block = exit_block
        self.locals = {}

class Emitter:

    def __init__(self, ctx, mod):
        self.ctx = ctx
        self.module = mod
        self.type_factory = llvm.TypeFactory(self.ctx)
        self.const_factory = llvm.ConstFactory(self.ctx)

    def generate(self, ast_root):
        self.functions = {}
        self.stack = []
        self.visit(ast_root)

        ftype = self.type_factory.function(
            ret = self.type_factory.void(),
            params = [self.type_factory.int32()]
        )
        fn = self.module.add_function('exit', ftype, 'external')

    @property
    def function_context(self):
        return self.stack[-1]

    @property
    def fn(self):
        return self.function_context.fn

    @property
    def locals(self):
        return self.function_context.locals

    @property
    def builder(self):
        return self.function_context.builder

    def visit_ModuleEntry(self, node):
        return self.visit(node.entry)

    def visit_Block(self, node):
        #self.fn.add_block(node.scope.path)
        for stmt in node.statements:
            self.visit(stmt)

    def _find_function(self, function_node):
        if function_node not in self.functions:
            self.visit(function_node)
        return self.functions[function_node]


    def _make_type(self, type):
        if type.name == 'int':
            return self.type_factory.int64()
        elif type.name == 'void':
            ret = self.type_factory.void()
            return ret

    def visit_FunctionCall(self, node):
        fn = self._find_function(node.fn)
        args = [self.visit(arg) for arg in node.args]
        self.builder.call(fn, args)

    @contextlib.contextmanager
    def push_fn(self, fn, entry, exit):
        with entry.builder() as builder:
            self.stack.append(
                FunctionContext(
                    fn = fn,
                    builder = builder,
                    entry_block = entry,
                    exit_block = exit,
                )
            )
            self.builder.position_at_end()
            yield
        self.stack.pop()

    def visit_Function(self, node):
        assert node not in self.functions
        sig = self.type_factory.function(
            self._make_type(node.return_type),
            [self._make_type(arg.type) for arg in node.args]
        )
        fn = self.module.add_function(node.name, sig)
        if node.name == 'main': # XXX
            fn.linkage = 'external'
        entry_block = fn.add_block('entry')
        exit_block = fn.add_block('exit')
        self.functions[node] = fn
        void_ret = (node.return_type.name == 'void') # XXX
        with self.push_fn(fn, entry_block, exit_block):
            for var in node.args:
                self.locals[var.id] = self.builder.alloca(
                    self._make_type(var.type),
                    var.id
                )
            if not void_ret:
                self.locals['#retval'] = self.builder.alloca(
                    self._make_type(node.return_type),
                    '#retval'
                )
            self.visit(node.body)
            self.builder.branch(exit_block)
            with exit_block.builder() as builder:
                builder.position_at_end()
                if void_ret:
                    builder.ret_void()
                else:
                    retval = builder.load(self.locals['#retval'], '#retval')
                    builder.ret(retval)
        return fn

    def visit_Extern(self, node):
        sig = self.type_factory.function(
            self._make_type(node.return_type),
            [self._make_type(arg.type) for arg in node.args]
        )
        fn = self.module.add_function(node.name, ftype, 'external')
        return fn

    def visit_Variable(self, node):
        return self.builder.load(self.locals[node.id], node.id)

    def visit_PrimaryOperator(self, node):
        lhs, rhs = map(self.visit, node.args)
        return self.builder.add(lhs, rhs, "")

    def visit_Return(self, node):
        value = self.visit(node.value)
        self.builder.store(value, self.locals['#retval'])
        self.builder.branch(self.function_context.exit_block)

    def visit_LiteralInteger(self, node):
        return self.const_factory.int(
            self._make_type(node.type),
            node.value
        )

    def visit(self, node):
        cls = node.__class__.__name__
        method = getattr(self, 'visit_' + cls)
        return method(node)
