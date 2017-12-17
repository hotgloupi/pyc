
class Type:
    def __init__(self, name):
        assert isinstance(name, str)
        self.name = name

    def __eq__(self, other):
        return self.name == other.name

    def __hash__(self):
        return hash(self.name)

    def __str__(self):
        return '<%s %s>' % (self.__class__.__name__, self.name)


class App(Type):
    def __init__(self, parameters_type, return_type):
        self.name = '(%s) -> %s' % (
            ', '.join(t.name for t in parameters_type),
            return_type.name
        )
        self.parameters_type = parameters_type
        self.return_type = return_type


class Array(Type):
    def __init__(self, element_type, size):
        assert isinstance(element_type, Type)
        self.element_type = element_type
        self.size = size
        self.name = '%s[%s]' % (element_type.name, size)
