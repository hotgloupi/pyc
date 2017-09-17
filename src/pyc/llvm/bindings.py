import os.path
import cffi
import sys

from . import include

__all__ = [
    'Context',
    'Type',
    'Value',
    'Module',
    'TypeFactory',
    'ConstFactory',
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
        if sys.platform == 'darwin':
            lib_name = 'libLLVM-C.dylib'
        else:
            lib_name = 'libLLVM-4.0.so'
        lib = ffi.dlopen(os.path.join(script_dir, 'lib', lib_name))

        cls.pass_registry = lib.LLVMGetGlobalPassRegistry()
        lib.LLVMInitializeCore(cls.pass_registry)
        cls.__lib = (ffi, lib)
        return cls.__lib


class Context:
    def __init__(self):
        self.ffi, self.lib = LLVM.open()

    def __enter__(self):
        self.ref = self.lib.LLVMContextCreate()
        return self

    def __exit__(self, *exc):
        #print("DELETE CONTEXT")
        self.lib.LLVMContextDispose(self.ref)
        self.ref = None


class Module:
    def __init__(self, name, context):
        self.context = context
        self.name = name

    def __enter__(self):
        self.ref = self.context.lib.LLVMModuleCreateWithNameInContext(
            self.name.encode('utf8'),
            self.context.ref
        )
        return self

    def __exit__(self, *exc):
        #print("DELETE MODULE")
        self.context.lib.LLVMDisposeModule(self.ref)
        self.ref = None

    def add_function(self, name, type):
        assert self.context.ref == type.context.ref
        return Function(
            ref = self.context.lib.LLVMAddFunction(
                self.ref,
                name.encode('utf8'),
                type.ref
            ),
            type = type,
            module = self,
        )

    def __str__(self):
        cstr = self.context.lib.LLVMPrintModuleToString(self.ref)
        res = self.context.ffi.string(cstr).decode('utf-8')
        self.context.lib.LLVMDisposeMessage(cstr)
        return res


class TypeFactory:
    def __init__(self, context):
        self.context = context

    def void(self):
        return Type(
            ref = self.context.lib.LLVMVoidTypeInContext(self.context.ref),
            context = self.context,
        )

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
        cparams = self.context.ffi.new(
            'LLVMTypeRef[]',
            [t.ref for t in params]
        )
        return Type(
            ref = self.context.lib.LLVMFunctionType(
                ret.ref,
                cparams,
                len(params),
                is_vararg
            ),
            context = self.context
        )


class ConstFactory:

    def __init__(self, context):
        self.context = context

    def int(self, type, value, sign_extend = True):
        return Value(
            ref = self.context.lib.LLVMConstInt(
                type.ref,
                value,
                sign_extend
            ),
            type = type,
        )

class Type:
    def __init__(self, ref, context):
        self.ref = ref
        self.context = context

class Value:
    def __init__(self, ref, type):
        self.ref = ref
        self.type = type

    @property
    def context(self):
        return self.type.context

class Function(Value):

    def __init__(self, ref, type, module):
        super().__init__(ref, type)
        self.module = module

    def add_block(self, name):
        return Block(
            ref = self.context.lib.LLVMAppendBasicBlockInContext(
                self.context.ref,
                self.ref,
                name.encode('utf-8')
            ),
            function = self,
        )

    @property
    def last_block(self):
        return Block(
            ref = self.context.lib.LLVMGetLastBasicBlock(self.ref),
            function = self,
        )

    @property
    def first_block(self):
        return Block(
            ref = self.context.lib.LLVMGetFirstBasicBlock(self.ref),
            function = self,
        )

class Block:
    def __init__(self, ref, function):
        self.ref = ref
        self.function = function

    def builder(self):
        return Builder(self)

    @property
    def context(self):
        return self.function.context

    @property
    def name(self):
        return self.context.ffi.string(
            self.context.lib.LLVMGetBasicBlockName(self.ref)
        ).decode('utf-8')

class Builder:
    def __init__(self, block):
        self.block = block

    @property
    def context(self):
        return self.block.context

    def position_at_end(self):
        self.context.lib.LLVMPositionBuilderAtEnd(self.ref, self.block.ref)

    def __enter__(self):
        self.ref = self.context.lib.LLVMCreateBuilderInContext(self.context.ref)
        return self

    def __exit__(self, *exc):
        #print("DELETE BUILDER")
        self.context.lib.LLVMDisposeBuilder(self.ref)
        self.ref = None

    def _inst(self, name, *args):
        fn = getattr(
            self.context.lib,
            'LLVMBuild' + name
        )
        return Instruction(
            builder = self,
            ref = fn(self.ref, *args)
        )

    def call(self, fn, args, name = None):
        cargs = self.context.ffi.new(
            'LLVMValueRef[]',
            [t.ref for t in args]
        )
        return self._inst(
            'Call',
            fn.ref,
            cargs,
            len(args),
            name is not None and name or b""
        )


    def alloca(self, type, name):
        return self._inst(
            'Alloca',
            type.ref,
            name.encode('utf8')
        )

    def load(self, value, name):
        return self._inst(
            'Load',
            value.ref,
            name.encode('utf8')
        )

    def store(self, value, ptr):
        return self._inst(
            'Store',
            value.ref,
            ptr.ref
        )

    def add(self, lhs, rhs, name):
        return self._inst(
            'Add',
            lhs.ref,
            rhs.ref,
            name.encode('utf8')
        )

    def branch(self, block):
        return self._inst(
            'Br',
            block.ref
        )

    def ret(self, value):
        return self._inst(
            'Ret',
            value.ref
        )


class Instruction:
    def __init__(self, builder, ref):
        self.builder = builder
        self.ref = ref

    @property
    def context(self):
        return self.builder.context

