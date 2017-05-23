
from .module import Module

from ..source import Manager as SourceManager

__all__ = ['Manager']

class Manager:

    def __init__(self):
        self.paths = []
        self.sources = SourceManager()
        self.cache = {}

    def add_path(self, path):
        pass

    def load_from_file(self, path, module_name):
        module = self.cache.get(module_name)
        if module is None:
            self.cache[module_name] = module = Module(
                name = module_name,
                source = self.sources.load(path),
                manager = self,
            )
        return module

    def load_absolute(self, module_name):
        names = module_name.split('.')

    def load_relative(self, module_name, directory):
        pass
