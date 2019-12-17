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


# setup the argument and return types for Initialize
_LIB.Initialize.argtypes = None
_LIB.Initialize.restype = ctypes.c_void_p
# setup the argument and return types for Delete
_LIB.Delete.argtypes = [ctypes.c_void_p]
_LIB.Delete.restype = None


class LimitOrderBook:
    """The Limit Order Book (LOB)."""

    def __init__(self):
        """Initialize a new limit order book."""
        self._book = _LIB.Initialize()

    def __del__(self):
        """Delete this limit order book."""
        _LIB.Delete(self._book)

    def limit_sell(self, size, price, arrival):
        """
        """

    def limit_buy(self, size, price, arrival):
        """
        """

    def limit(self, side, size, price, arrival):
        """
        """

    def has(self, order_id):
        """
        """

    def get(self, order_id):
        """
        """

    def cancel(self, order_id):
        """
        """

    def market_sell(self, size, arrival):
        """
        """

    def market_buy(self, size, arrival):
        """
        """

    def market(self, side, size, arrival):
        """
        """

    def best_sell(self):
        """
        """

    def best_buy(self):
        """
        """

    def best(self):
        """
        """

    def volume_sell(self, price=None):
        """
        """

    def volume_buy(self, price=None):
        """
        """

    def volume(self, price=None):
        """
        """

    def size_at(self):
        """
        """

    def size_sell(self):
        """
        """
    def size_buy(self):
        """
        """

    def size(self, price=None):
        """
        """


# explicitly define the outward facing API of this module
__all__ = [LimitOrderBook.__name__]
