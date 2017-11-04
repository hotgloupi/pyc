from pprint import pformat

from ..tools import Formatter

from .iter import iter_fields
from .nodes import Node

__all__ = ['dump']

class Formatter:

    def __init__(self, indent_str = ' ', additional_fields = []):
        self.indent_str = ' '
        self.additional_fields = additional_fields

    def __call__(self, node, indent = 0):
        if isinstance(node, str):
            return repr(node)
        elif isinstance(node, Node):
            cls = node.__class__
            res = cls.kind + '.' + cls.__name__ + ':'
            fields = []
            for k, v in iter_fields(node, self.additional_fields):
                key = k + ':'
                field = self(v, indent + 1)
                sep = ' '
                #if field and field[0] == ' ': sep = '\n'
                #if '\n' in field:
                #    sep = '\n'
                fields.append(key + sep + field)
            sep = ' '
            if len(fields) > 1:
                sep = '\n'
                fields = '\n'.join(self.indent(field, indent + 1) for field in fields)
            elif fields:
                fields = fields[0]
            else:
                fields = '{}'
            return res + sep + fields
        elif hasattr(node, '__iter__'):
            res = '['
            for el in node:
                res += '\n' + self.indent(self(el, indent + 2), indent + 1)
            if len(res) > 1:
                res += '\n' + self.indent(']', indent)
            else:
                res += ']'
            return res
        else:
            return str(node)

    def indent(self, str, indent):
        return self.indent_str * indent + str

def dump(node, **kw):
    return Formatter(**kw)(node)

def dump2(node: Node, **kw):
    def transform(node):
        if isinstance(node, Node):
            fields = dict((k, transform(v)) for k, v in iter_fields(node))
            cls = node.__class__
            return (cls.kind + '.' + cls.__name__, fields)
        elif isinstance(node, str):
            return repr(node)
        elif hasattr(node, '__iter__'):
            return [transform(n) for n in node]
        return node
    return Formatter()(transform(node))
    return pformat(transform(node), **kw)
