from .emitter import Emitter

__all__ = [
    'compile',
]

def compile(node, module_name):
    emitter = Emitter(module_name = module_name)
    emitter.visit(node)
    return emitter.module
