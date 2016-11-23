
from .iter import iter_child_nodes

__all__ = ['Visitor']

class Visitor:

    def visit(self, node):
        cls = node.__class__.__name__
        method = getattr(self, 'visit_' + cls, None)
        if method is not None:
            return method(node)
        else:
            return self.visit_children(node)

    def visit_children(self, node):
        for child in iter_child_nodes(node):
            self.visit(child)

