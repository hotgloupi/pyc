from .. import llvm
from .emitter import Emitter

__all__ = [
    'compile',
]

def compile(node, module_name):
    with llvm.Context() as ctx:
        with llvm.Module(module_name, ctx) as mod:
            emitter = Emitter(ctx, mod)
            emitter.generate(node)
            return str(mod)
