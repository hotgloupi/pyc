import argparse
import os
import sys
import logging

from . import ast
from .module import Manager
from . import codegen
from .core.dump import dump as dump_core_lang

FILE = os.path.abspath(__file__)
FILE_DIR = os.path.dirname(FILE)
DEFAULT_PYC_DIR = os.path.join(FILE_DIR, '..')
DEFAULT_STDLIB_DIR = os.path.join(FILE_DIR, '../../lib')

log = logging.getLogger(__name__)

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
        '--core-lang',
        help = 'Dump the core AST in a human readable C-like format',
        action = 'store_true',
    )

    parser.add_argument(
        '--llvm-ir',
        help = 'Dump LLVM IR',
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

    parser.add_argument(
        '--output', '-o',
        default = sys.stdout,
        type = argparse.FileType(mode = 'w')
    )

    parser.add_argument(
        '--verbose', '-v',
        action = 'count'
    )

    return parser

def setup_logger(args):
    max_len = max(map(len, logging.Logger.manager.loggerDict.keys()))
    formatter = logging.Formatter(
        '%(relativeCreated)04d: %(name)-' + str(max_len) + 's - [%(levelname)-5s] - %(message)s'
    )

    handler = logging.StreamHandler()
    handler.setLevel(logging.DEBUG)
    handler.setFormatter(formatter)
    log.addHandler(handler)

    level = logging.WARNING
    if args.verbose:
        if args.verbose > 1:
            level = logging.DEBUG
        else:
            level = logging.INFO
    log.setLevel(level)

def main():
    argument_parser = make_argument_parser()
    args = argument_parser.parse_args()

    setup_logger(args)

    file = os.path.abspath(args.file[0])
    dir = os.path.dirname(file)
    manager = Manager(paths = [dir, args.stdlib_dir, args.pyc_dir])
    builtins = manager.load_absolute(['builtins'])
    module = manager.load_from_file(file, '__main__', builtins)

    if args.tokens:
        print(module.tokens, file = args.output)
        return

    if args.python_ast:
        print(ast.dump(module.python_ast), file = args.output)
        return

    if args.core_ast:
        print(
            ast.dump(module.core_ast, additional_fields = ['type']),
            file = args.output
        )
        return

    if args.core_lang:
        dump_core_lang(module.core_ast, file = args.output)
        return

    ir = codegen.compile(module.core_ast, os.path.abspath(args.file[0]))
    if args.llvm_ir:
        print(ir, file = args.output)
        return
