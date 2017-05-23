import argparse
import sys
import os

from . import ast

from .module import Manager

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

    manager = Manager()
    module = manager.load_from_file(os.path.abspath(args.file[0]), '__main__')
    print(ast.dump(module.core_ast))
