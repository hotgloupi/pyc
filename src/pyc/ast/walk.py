from .nodes import Node

__all__ =  ['walk']

def walk(node : Node, ancestors : tuple = ()):
    if not isinstance(node, Node):
        return
    yield ancestors, node
    ancestors += (node, )
    for field in node.fields:
        child = getattr(node, field)
        if hasattr(child, '__iter__'):
            for n in child:
                yield from walk(n, ancestors = ancestors)
        else:
            yield from walk(child, ancestors = ancestors)

