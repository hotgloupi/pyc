
from .. import parser
from ..parser import lexer
from .. import ast
from .. import core

__all__ = ['Module']

class Module:

    def __init__(self, name, source, manager):
        self.name = name
        self.source = source
        self.manager = manager

    @property
    def tokens(self):
        return lexer.lex_file_input(self.source.data)

    @property
    def python_ast(self):
        return parser.parse(self.tokens)

    @property
    def core_ast(self):
        return core.convert(self.python_ast)
