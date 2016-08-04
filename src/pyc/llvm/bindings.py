import cffi

from . import include

__all__ = ['ffi']
ffi = cffi.FFI()
ffi.cdef(include.read('Types.h'))
ffi.cdef(include.read('Core.h'))
