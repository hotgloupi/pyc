
import os

from .source import Source

__all__ = ['Manager']

class Manager:

    def __init__(self):
        self.cache = {}

    def exists(self, path):
        if not os.path.isabs(path):
            raise Exception("The path '%s' is not absolute" % path)
        return os.path.exists(path)

    def load(self, path):
        if not self.exists(path):
            raise FileNotFound("The file '%s' does not exist" % path)

        path = os.path.normpath(path)

        src = self.cache.get(path)
        if src is None:
            self.cache[path] = src = Source(path)
        return src
