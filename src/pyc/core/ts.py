
class Type:
    def __init__(self, name):
        assert isinstance(name, str)
        self.name = name

    def __eq__(self, other):
        return self.name == other.name

    def __hash__(self):
        return hash(self.name)

    def __str__(self):
        return '<Type %s>' % self.name
