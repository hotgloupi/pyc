# builtins

from __pyc__ import extern, int, void_p, size_t, ssize_t

@extern
def write(filedes: int, buf: void_p, size: size_t) -> ssize_t:
    pass

def print(s):
    write(1, s, 5)
