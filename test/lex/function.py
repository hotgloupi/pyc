

def f():
    pass

def f(): pass

def f(a): pass
def f(a, b): pass
def f(a, b = 12): pass

def f(a = 1, b = 2, c = d):
    def g():
        pass
    return g()

def f(a = 1, b):
    pass
