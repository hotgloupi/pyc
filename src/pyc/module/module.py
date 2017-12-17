import logging

from .. import parser
from ..parser import lexer
from .. import ast
from .. import core
from ..tools import cached_property

__all__ = ['Module']

log = logging.getLogger(__name__)

class Module:

    def __init__(self, name, source, manager, builtins):
        self.name = name
        self.source = source
        self.manager = manager
        self.builtins = builtins

    @cached_property
    def tokens(self):
        log.info("Lexing source '%s'", self.source)
        return lexer.lex_file_input(self.source.data)

    @cached_property
    def python_ast(self):
        log.info("Parsing source '%s'", self.source)
        return parser.parse(self.tokens)

    @cached_property
    def core_ast(self):
        log.info("Converting AST of '%s'", self.source)
        return core.convert(self, self.python_ast, self.builtins)

    def __str__(self):
        return '<Module %s>' % self.name
