import logging
import contextlib
from collections import namedtuple

from .. import llvm
from .. import core


LOG = logging.getLogger(__name__)

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
    """Class responsible for emitting LLVM IR from a core AST node.
    """

    def __init__(self, ctx : llvm.Context, mod : llvm.Module):
        self.ctx = ctx
        self.module = mod
        self.type_factory = llvm.TypeFactory(self.ctx)
        self.const_factory = llvm.ConstFactory(self.ctx)
        self.types = {
            'int': self.type_factory.int32(),
            'ssize_t': self.type_factory.int64(),
            'size_t': self.type_factory.int64(),
            'void': self.type_factory.void(),
            'void_p': self.type_factory.void_p(),
            'char_p': self.type_factory.int_p(8),
            'char': self.type_factory.int(8),
        }

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
            # the visit_Function registers itself the node to allow recursive
            # functions
            self.visit(function_node)
        return self.functions[function_node]


    def _make_type(self, type):
        ret = self.types.get(type.name)
        if ret is not None:
            return ret
        if isinstance(type, core.ts.Array):
            ret = self.types[type.name] = self.type_factory.array(
                self._make_type(type.element_type),
                type.size
            )
            return ret
        raise Exception("Unknown type '%s'" % type)

    def visit_FunctionCall(self, node):
        fn = self._find_function(node.fn)
        args = [self.visit(arg) for arg in node.args]
        print("FunctionCall", fn, 'with', *args)
        print("build it")
        ret = self.builder.call(fn, args)
        print("Done")
        return ret

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
            [self._make_type(parameter.type) for parameter in node.parameters]
        )
        fn = self.module.add_function(node.name, sig)
        if node.name == 'main': # XXX
            fn.linkage = 'external'
        entry_block = fn.add_block('entry')
        exit_block = fn.add_block('exit')
        self.functions[node] = fn
        void_ret = (node.return_type.name == 'void') # XXX
        with self.push_fn(fn, entry_block, exit_block):
            for node_param, llvm_param in zip(node.parameters, fn.params):
                self.locals[node_param.name] = llvm_param
            if not void_ret:
                self.locals['#retval'] = self.builder.alloca(
                    self._make_type(node.return_type),
                    '#retval'
                )
            if node.body is not None:
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
        print(node)
        return self.locals[node.id]
        #return self.builder.load(self.locals[node.id], node.id)

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

    def visit_LiteralString(self, node):
        return self.const_factory.string(
            self._make_type(node.type),
            node.value,
            null_terminated = True,
        )

    def visit_Cast(self, node):
        to_type = node.to_type
        value = self.visit(node.value)

    def visit(self, node):
        cls = node.__class__.__name__
        method = getattr(self, 'visit_' + cls, None)
        if method is None:
            raise Exception("Unhandled core node '%s'" % node)
        return method(node)
