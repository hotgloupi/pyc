
import os

from .module import Module

from ..source import Manager as SourceManager
from ..core import internal

__all__ = ['Manager']

class InternalModule:
    def __init__(self, module):
        self.core_ast = module

class Manager:

    def __init__(self, paths = None):
        self.paths = paths or []
        self.sources = SourceManager()
        self.cache = {}
        self.internal_module = InternalModule(internal.intrinsics)

    def add_path(self, path):
        pass

    def load_from_file(self, path, module_name, builtins = None):
        module = self.cache.get(module_name)
        if module is None:
            print("loading module", module_name, "from", path)
            self.cache[module_name] = module = Module(
                name = module_name,
                source = self.sources.load(path),
                manager = self,
                builtins = builtins
            )
        return module

    def load_absolute(self, module_path, builtins = None):
        name = '.'.join(module_path)
        if name == '__pyc__':
            return self.internal_module
        # XXX try cache here
        assert isinstance(module_path, list)
        candidates = [
            '/'.join(module_path) + '.py'
        ]
        for candidate in candidates:
            for dir in self.paths:
                path = os.path.join(dir, candidate)
                if os.path.isfile(path):
                    return self.load_from_file(path, name, builtins)
        raise Exception("Couldn't import '%s'" % name)

    def load_relative(self, module_name, directory):
        pass
