
from .. import parser
from ..parser import lexer
from .. import ast
from .. import core
from ..tools import cached_property

__all__ = ['Module']

class Module:

    def __init__(self, name, source, manager, builtins):
        self.name = name
        self.source = source
        self.manager = manager
        self.builtins = builtins

    @cached_property
    def tokens(self):
        #print("Lexing source", self.source)
        return lexer.lex_file_input(self.source.data)

    @cached_property
    def python_ast(self):
        #print("Parsing source", self.source)
        return parser.parse(self.tokens)

    @cached_property
    def core_ast(self):
        print("Converting AST of", self.source)
        return core.convert(self, self.python_ast, self.builtins)
