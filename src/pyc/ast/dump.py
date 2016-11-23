from .iter import iter_fields
from .nodes import Node

from pprint import pformat

__all__ = ['dump']

def dump(node : Node, **kw):
    def transform(node):
        if isinstance(node, Node):
            fields = dict((k, transform(v)) for k, v in iter_fields(node))
            return (node.__class__.__name__, fields)
        elif isinstance(node, str):
            return repr(node)
        elif hasattr(node, '__iter__'):
            return [transform(n) for n in node]
        return node
    kw.setdefault('indent', 1)
    kw.setdefault('width', 120)
    return pformat(transform(node), **kw)
