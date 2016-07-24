import argparse

from .parser import parse_file

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
    ast = parse_file(file = args.file[0])
