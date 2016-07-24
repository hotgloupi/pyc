
from .lexer import lex_file_input

class Parser:
    def push(self, tokens):
        pass

def parse_file(file = None):
    assert file is not None
    with open(file) as f:
        return lex_file_input(f.read())
