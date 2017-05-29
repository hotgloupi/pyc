from .. import llvm

__all__ = [
    'Emitter',
]

class Emitter:
    def __init__(self):
        self.ctx = llvm.Context()
        self.module = llvm.Module('__main__', self.ctx)
        self.type_factory = llvm.TypeFactory(self.ctx)
        main_sig = self.type_factory.function(self.type_factory.int32(), ())
        self.main = self.module.add_function('main', main_sig)
        self.main_entry = self.main.add_block("entry")
        self.main_exit = self.main.add_block("exit")
        print(self.main_exit.name)
        print(self.module)

    def visit_Block(self, node):
        pass

    def visit(self, node):
        cls = node.__class__.__name__
        method = getattr(self, 'visit_' + cls)
        method(node)
