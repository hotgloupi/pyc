from .nodes import Node
from .iter import iter_child_nodes

__all__ =  ['walk']

def walk(node : Node, ancestors : tuple = ()):
    if not isinstance(node, Node):
        return
    yield ancestors, node
    ancestors += (node, )
    for child in iter_child_nodes(node):
        yield from walk(child, ancestors = ancestors)

