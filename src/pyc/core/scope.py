
__all__ = ['Scope']

class Scope():
    __index = [1]

    def __init__(self, parent, name = None):
        self.parent = parent
        self.values = {}
        if name is None:
            self.name = str(self.__index[0])
            self.__index[0] += 1
        else:
            self.name = name

    def __setitem__(self, key, value):
        self.values[key] = value

    def __getitem__(self, key):
        value = self.find(key)
        if value is None:
            raise Exception("Couln't find '%s'" % key)
        return value

    def find(self, key):
        value = self.values.get(key)
        if value is not None:
            return value
        if self.parent is not None:
            return self.parent.find(key)
        return None

    @property
    def path(self):
        if self.parent is not None:
            return self.parent.path + '.' + self.name
        return self.name

    def __repr__(self):
        return '@' + self.path

