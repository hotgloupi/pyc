from contextlib import contextmanager
from .bindings import ffi
lib = ffi.dlopen('/usr/lib/llvm-3.8/lib/libLLVM-3.8.so')

pass_registry = lib.LLVMGetGlobalPassRegistry()
lib.LLVMInitializeCore(pass_registry)

class Context:
    def __init__(self, use_global = False):
        self.is_global = use_global
        if use_global:
            self.ref = lib.LLVMGetGlobalContext()
        else:
            self.ref = lib.LLVMCreateContext()

    def __del__(self):
        if not self.is_global:
            lib.LLVMContextDispose(self.ref)


    __global_context = None
    @classmethod
    def get_global(cls):
        if cls.__global_context is None:
            cls.__global_context = Context(use_global = True)
        return cls.__global_context

class Module:
    def __init__(self, name, context = None):
        if context is None:
            context = Context.get_global()
        self.context = context
        self.ref = lib.LLVMModuleCreateWithNameInContext(name.encode('utf8'), context.ref)

    def __del__(self):
        lib.LLVMDisposeModule(self.ref)


    def add_function(self, name, sig):
        assert self.context.ref == sig.context.ref
        return Value(
            ref = lib.LLVMAddFunction(self.ref, name.encode('utf8'), sig.ref),
            context = self.context
        )

class TypeFactory:
    def __init__(self, context = None):
        if context is None:
            context = Context.get_global()
        self.context = context

    def int(self, bits):
        return Type(
            ref = lib.LLVMIntTypeInContext(self.context.ref, bits),
            context = self.context
        )

    def int1(self): return self.int(1)
    def int8(self): return self.int(8)
    def int16(self): return self.int(16)
    def int32(self): return self.int(32)
    def int64(self): return self.int(64)
    def int128(self): return self.int(128)

    def half(self):
        return Type(
            ref = lib.LLVMHalfTypeInContext(self.context.ref),
            context = self.context
        )

    def float(self):
        return Type(
            ref = lib.LLVMFloatTypeInContext(self.context.ref),
            context = self.context
        )

    def double(self):
        return Type(
            ref = lib.LLVMDoubleTypeInContext(self.context.ref),
            context = self.context
        )

    def x86fp80(self):
        return Type(
            ref = lib.LLVMX86FP80TypeInContext(self.context.ref),
            context = self.context
        )

    def fp128(self):
        return Type(
            ref = lib.LLVMFP128TypeInContext(self.context.ref),
            context = self.context
        )

    def ppcfp128(self):
        return Type(
            ref = lib.LLVMPPCFP128TypeInContext(self.context.ref),
            context = self.context
        )

    def function(self, ret, params, is_vararg = False):
        cparams = ffi.new('LLVMTypeRef[]', [t.ref for t in params])
        return Type(
            ref = lib.LLVMFunctionType(ret.ref, cparams, len(params), is_vararg),
            context = self.context
        )


class Type:
    def __init__(self, ref, context):
        self.ref = ref
        self.context = context

class Value:
    def __init__(self, ref, context):
        self.ref = ref
        self.context = context

factory = TypeFactory()
fn_type = factory.function(factory.int32(), [factory.int32()])

m = Module('test')
fn = m.add_function('test', fn_type)
del fn
del factory
del fn_type
del m
import gc
gc.collect()
