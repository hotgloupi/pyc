
from pyc.tools import cached_property

__all__ = ['Source']

class Source:
    def __init__(self, path, data = None):
        self.path = path
        self.__data = data

    @property
    def data(self):
        if self.__data is None:
            #print("Read data from", self)
            with open(self.path) as f:
                self.__data = f.read()
        return self.__data

    def __str__(self):
        return "<Source %s>" % (self.path)
