
from .nodes import Node

__all__ = ['iter_fields', 'iter_child_nodes']

def iter_fields(node: Node, additional_fields = []):
    for field in node.fields:
        yield (field, getattr(node, field))
    for field in additional_fields:
        if field in node.fields: continue
        yield (field, getattr(node, field))

def iter_child_nodes(node : Node):
    for field in node.fields:
        child = getattr(node, field)
        if hasattr(child, '__iter__'):
            for n in child:
                yield n
        else:
            yield child
