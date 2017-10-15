import argparse
import os

from . import ast
from .module import Manager
from . import codegen

FILE = os.path.abspath(__file__)
FILE_DIR = os.path.dirname(FILE)
DEFAULT_PYC_DIR = os.path.join(FILE_DIR, '..')
DEFAULT_STDLIB_DIR = os.path.join(FILE_DIR, '../../lib')

def make_argument_parser():
    parser = argparse.ArgumentParser(prog = 'pyc')

    parser.add_argument(
        'file',
        help = 'Files to compile',
        action = 'append',
        default = []
    )

    parser.add_argument(
        '--tokens',
        help = 'Dump lexemes',
        action = 'store_true',
    )

    parser.add_argument(
        '--python-ast',
        help = 'Dump the AST',
        action = 'store_true',
    )

    parser.add_argument(
        '--core-ast',
        help = 'Dump the core AST',
        action = 'store_true',
    )

    parser.add_argument(
        '--llvm-ir',
        help = 'Dump the core AST',
        action = 'store_true',
    )

    parser.add_argument(
        '--stdlib-dir',
        default = DEFAULT_STDLIB_DIR
    )
    parser.add_argument(
        '--pyc-dir',
        default = DEFAULT_PYC_DIR
    )
    return parser

def main():
    argument_parser = make_argument_parser()
    args = argument_parser.parse_args()

    file = os.path.abspath(args.file[0])
    dir = os.path.dirname(file)
    manager = Manager(paths = [dir, args.stdlib_dir, args.pyc_dir])
    builtins = manager.load_absolute(['builtins'])
    module = manager.load_from_file(file, '__main__', builtins)

    if args.tokens:
        from pprint import pprint
        pprint(module.tokens)
        return

    if args.python_ast:
        print(ast.dump(module.python_ast))
        return

    if args.core_ast:
        print(ast.dump(module.core_ast))
        return

    ir = codegen.compile(module.core_ast, os.path.abspath(args.file[0]))
    if args.llvm_ir:
        print(ir)
        return
