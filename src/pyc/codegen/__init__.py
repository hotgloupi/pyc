"""LLVM IR generation
"""
from .. import llvm
from .emitter import Emitter

__all__ = ['compile']


def compile(node : "Node", module_name: str) -> str:
    """Convert a core AST node to LLVM IR string.

    Args:
        node: The core AST root node
        module_name: LLVM module name

    Returns: LLVM IR string representation
    """
    with llvm.Context() as ctx:
        with llvm.Module(module_name, ctx) as mod:
            emitter = Emitter(ctx, mod)
            emitter.generate(node)
            return str(mod)
