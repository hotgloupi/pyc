import argparse

from .parser import parse
from .parser.lexer import lex_file_input

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
    import sys
    argument_parser = make_argument_parser()
    args = argument_parser.parse_args()
    with open(args.file[0]) as f:
        tokens = lex_file_input(f.read())
    ast = parse(tokens)
    print(ast)
