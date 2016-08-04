
import os

__all__ = ['read']

script_dir = os.path.dirname(os.path.abspath(__file__))
def read(name):
    with open(os.path.join(script_dir, name)) as f:
        return f.read()
