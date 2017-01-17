import argparse
import sys

from .parser import parse
from .parser.lexer import lex_file_input
from . import ast
from . import core

def make_argument_parser():
    parser = argparse.ArgumentParser(prog = 'pyc')

    parser.add_argument(
        'file',
        help = 'Files to compile',
        action = 'append',
        default = []
    )

    return parser

def main():
    argument_parser = make_argument_parser()
    args = argument_parser.parse_args()
    with open(args.file[0]) as f:
        tokens = lex_file_input(f.read())

    print("######### Python AST")
    py_ast = parse(tokens)
    print(ast.dump(py_ast))

    print("######### Core AST")
    core_ast = core.convert(py_ast)
    print("######### Core AST created:", core_ast)
    print(ast.dump(core_ast))
