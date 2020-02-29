"""Module class
"""
import logging

from .. import parser
from ..parser import lexer
from .. import core
from ..tools import cached_property

__all__ = ['Module']

LOG = logging.getLogger(__name__)


class Module:
    """Representation of a python module.

    This class is responsible for various conversions from the source file to
    tokens, to the python AST and finally to the core AST.
    """

    def __init__(self, name : str, source : "Source", manager : "Manager", builtins : "Module"):
        self.name = name
        self.source = source
        self.manager = manager
        self.builtins = builtins

    @cached_property
    def tokens(self):
        """Lex the source file and returns a stream of tokens"""
        LOG.info("Lexing source '%s'", self.source)
        return lexer.lex_file_input(self.source.data)

    @cached_property
    def python_ast(self):
        """Convert the tokens to the python AST"""
        LOG.info("Parsing source '%s'", self.source)
        return parser.parse(self.tokens)

    @cached_property
    def core_ast(self):
        """Convert the python AST to the core AST"""
        LOG.info("Converting AST of '%s'", self.source)
        return core.convert(self, self.python_ast, self.builtins)

    def __str__(self):
        return '<Module %s>' % self.name

class InternalModule(Module):
    """Specialized module for intrinsics.

    This class only holds a core AST node defined programatically.
    """
    def __init__(self, node : core.ast.Node, manager : "Manager"):
        super().__init__(name="__pyc__", source=None, manager=manager, builtins=None)
        self.__core_ast = node

    @property
    def tokens(self): raise NotImplementedError()

    @property
    def python_ast(self): raise NotImplementedError()

    @property
    def core_ast(self):
        return self.__core_ast
