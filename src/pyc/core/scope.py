
__all__ = ['Scope']

class Scope():
    __index = [1]

    def __init__(self, parent, name = None, namespace = None):
        self.parent = parent
        self.namespace = {}
        if namespace:
            self.namespace.update(namespace)
        self.id = self.__index[0]
        self.__index[0] += 1
        self.name = name

    def __setitem__(self, key, value):
        self.namespace[key] = value

    def __getitem__(self, key):
        value = self.find(key)
        if value is None:
            raise Exception("Couln't find '%s'" % key)
        return value

    def find(self, key):
        value = self.namespace.get(key)
        if value is not None:
            return value
        if self.parent is not None:
            return self.parent.find(key)
        return None

    @property
    def path(self):
        if self.parent is not None:
            return self.parent.path + '.' + (self.name or str(self.id))
        return (self.name or str(self.id))

    @property
    def absolute_name(self):
        names = []
        if self.parent is not None:
            name = self.parent.absolute_name
            if name is not None:
                names.append(name)
        if self.name is not None:
            names.append(self.name)
        if names:
            return '.'.join(names)

    def __repr__(self):
        return '@' + self.path

