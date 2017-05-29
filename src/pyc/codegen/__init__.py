from .emitter import Emitter

__all__ = [
    'compile',
]

def compile(node):
    emitter = Emitter()
    return emitter.visit(node)
