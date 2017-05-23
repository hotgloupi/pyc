
from pyc.tools import cached_property

__all__ = ['Source']

class Source:
    def __init__(self, path):
        self.path = path

    @cached_property
    def data(self):
        with open(self.path) as f:
            return f.read()
