"""A CTypes interface to a Limit Order Book (LOB)."""
import ctypes
import os
import glob


# the path to the directory this file is in
_MODULE_PATH = os.path.dirname(__file__)
# the pattern to find the C++ shared object library
_SO_PATH = 'lib_lob*'
# the absolute path to the C++ shared object library
_LIB_PATH = os.path.join(_MODULE_PATH, _SO_PATH)
try:  # load the library from the shared object file
    _LIB = ctypes.cdll.LoadLibrary(glob.glob(_LIB_PATH)[0])
except IndexError:
    raise OSError('missing static lib_lob*.so library!')


class LimitOrderBook:
    pass


# explicitly define the outward facing API of this module
__all__ = [LimitOrderBook.__name__]
