import os.path
import cffi

from . import include

__all__ = [
    'Context',
    'Type',
    'Value',
    'Module',
    'TypeFactory',
]


class LLVM:
    __lib = None

    @classmethod
    def open(cls):
        if cls.__lib is not None:
            return cls.__lib
        ffi = cffi.FFI()
        ffi.cdef(include.read('Types.h'))
        ffi.cdef(include.read('Core.h'))

        script_dir = os.path.dirname(os.path.abspath(__file__))
        lib = ffi.dlopen(os.path.join(script_dir, 'lib/libLLVM-3.9.so'))

        cls.pass_registry = lib.LLVMGetGlobalPassRegistry()
        lib.LLVMInitializeCore(cls.pass_registry)
        cls.__lib = (ffi, lib)
        return cls.__lib


class Context:
    def __init__(self):
        self.ffi, self.lib = LLVM.open()
        self.ref = self.lib.LLVMCreateContext()

    def __del__(self):
        self.lib.LLVMContextDispose(self.ref)


class Module:
    def __init__(self, name, context):
        self.context = context
        self.ref = context.lib.LLVMModuleCreateWithNameInContext(
            name.encode('utf8'), context.ref)

    def __del__(self):
        self.context.lib.LLVMDisposeModule(self.ref)

    def add_function(self, name, sig):
        assert self.context.ref == sig.context.ref
        return Value(
            ref=self.context.lib.LLVMAddFunction(self.ref,
                                                 name.encode('utf8'), sig.ref),
            context=self.context)


class TypeFactory:
    def __init__(self, context):
        self.context = context

    def int(self, bits):
        return Type(
            ref=self.context.lib.LLVMIntTypeInContext(self.context.ref, bits),
            context=self.context)

    def int1(self):
        return self.int(1)

    def int8(self):
        return self.int(8)

    def int16(self):
        return self.int(16)

    def int32(self):
        return self.int(32)

    def int64(self):
        return self.int(64)

    def int128(self):
        return self.int(128)

    def half(self):
        return Type(
            ref=self.context.lib.LLVMHalfTypeInContext(self.context.ref),
            context=self.context)

    def float(self):
        return Type(
            ref=self.context.lib.LLVMFloatTypeInContext(self.context.ref),
            context=self.context)

    def double(self):
        return Type(
            ref=self.context.lib.LLVMDoubleTypeInContext(self.context.ref),
            context=self.context)

    def x86fp80(self):
        return Type(
            ref=self.context.lib.LLVMX86FP80TypeInContext(self.context.ref),
            context=self.context)

    def fp128(self):
        return Type(
            ref=self.context.lib.LLVMFP128TypeInContext(self.context.ref),
            context=self.context)

    def ppcfp128(self):
        return Type(
            ref=self.context.lib.LLVMPPCFP128TypeInContext(self.context.ref),
            context=self.context)

    def function(self, ret, params, is_vararg=False):
        cparams = self.context.ffi.new('LLVMTypeRef[]',
                                       [t.ref for t in params])
        return Type(
            ref=self.context.lib.LLVMFunctionType(ret.ref, cparams,
                                                  len(params), is_vararg),
            context=self.context)


class Type:
    def __init__(self, ref, context):
        self.ref = ref
        self.context = context


class Value:
    def __init__(self, ref, context):
        self.ref = ref
        self.context = context
