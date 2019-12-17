"""The Limit Order Book (LOB)."""
import ctypes
import os
import glob


LIB_PATH = os.path.join(os.path.dirname(__file__), 'lib_lob*')
try:  # load the library from the shared object file
    LIB = ctypes.cdll.LoadLibrary(glob.glob(LIB_PATH)[0])
except IndexError:
    raise OSError('missing static lib_lob*.so library!')

Size = ctypes.c_uint32
Price = ctypes.c_uint64
Volume = ctypes.c_uint32

# setup the argument and return types for Initialize
LIB.new_.argtypes = None
LIB.new_.restype = ctypes.c_void_p
# setup the argument and return types for Delete
LIB.delete_.argtypes = [ctypes.c_void_p]
LIB.delete_.restype = None



# setup the argument and return types for best_sell
LIB.best_sell.argtypes = [ctypes.c_void_p]
LIB.best_sell.restype = Price
# setup the argument and return types for best_buy
LIB.best_buy.argtypes = [ctypes.c_void_p]
LIB.best_buy.restype = Price
# setup the argument and return types for best
LIB.best.argtypes = [ctypes.c_void_p, ctypes.c_bool]
LIB.best.restype = Price

# setup the argument and return types for volume_sell_price
LIB.volume_sell_price.argtypes = [ctypes.c_void_p, Price]
LIB.volume_sell_price.restype = Volume
# setup the argument and return types for volume_sell
LIB.volume_sell.argtypes = [ctypes.c_void_p]
LIB.volume_sell.restype = Volume
# setup the argument and return types for volume_buy_price
LIB.volume_buy_price.argtypes = [ctypes.c_void_p, Price]
LIB.volume_buy_price.restype = Volume
# setup the argument and return types for volume_buy
LIB.volume_buy.argtypes = [ctypes.c_void_p]
LIB.volume_buy.restype = Volume
# setup the argument and return types for volume_price
LIB.volume_price.argtypes = [ctypes.c_void_p, Price]
LIB.volume_price.restype = Volume
# setup the argument and return types for volume
LIB.volume.argtypes = [ctypes.c_void_p]
LIB.volume.restype = Volume

# setup the argument and return types for size_at
LIB.size_at.argtypes = [ctypes.c_void_p, Price]
LIB.size_at.restype = Size
# setup the argument and return types for size_sell
LIB.size_sell.argtypes = [ctypes.c_void_p]
LIB.size_sell.restype = Size
# setup the argument and return types for size_buy
LIB.size_buy.argtypes = [ctypes.c_void_p]
LIB.size_buy.restype = Size
# setup the argument and return types for size
LIB.size.argtypes = [ctypes.c_void_p]
LIB.size.restype = Size


class LimitOrderBook:
    """The Limit Order Book (LOB)."""

    def __init__(self):
        """Initialize a new limit order book."""
        self._book = LIB.new_()

    def __del__(self):
        """Delete this limit order book."""
        LIB.delete_(self._book)

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
