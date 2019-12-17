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
        """Return the best sell price in the book."""
        return LIB.best_sell(self._book)

    def best_buy(self):
        """Return the best buy price in the book."""
        return LIB.best_buy(self._book)

    def best(self, side):
        """
        Return the best price for the given side.

        Args:
            side: the side of the book to get the best price for (False=sell)

        Returns:
            the best price for the given side of the book

        """
        return LIB.best(self._book, side)

    def volume_sell(self, price=None):
        """
        Return the volume of the sell side of the book at the given price.

        Args:
            price: the price to get the volume at

        Returns:
            the volume of orders at the given price

        Note:
            returns the total sell-side volume if price is `None`

        """
        if price is None:
            return LIB.volume_sell(self._book)
        return LIB.volume_sell_price(self._book, price)

    def volume_buy(self, price=None):
        """
        Return the volume of the buy side of the book at the given price.

        Args:
            price: the price to get the volume at

        Returns:
            the volume of orders at the given price

        Note:
            returns the total buy-side volume if price is `None`

        """
        if price is None:
            return LIB.volume_buy(self._book)
        return LIB.volume_buy_price(self._book, price)

    def volume(self, price=None):
        """
        Return the volume of the book at the given price.

        Args:
            price: the price to get the volume at

        Returns:
            the volume of orders at the given price

        Note:
            returns the total volume if price is `None`

        """
        if price is None:
            return LIB.volume(self._book)
        return LIB.volume_price(self._book, price)

    def size_at(self, price):
        """
        Return the size at the given limit price.

        Args:
            price: the price to get the size of the book for

        Returns:
            the size of the book at the given price

        """
        return LIB.size_at(self._book, price)

    def size_sell(self):
        """Return the size of the book on the sell side."""
        return LIB.size_sell(self._book)

    def size_buy(self):
        """Return the size of the book on the buy side."""
        return LIB.size_buy(self._book)

    def size(self):
        """Return the total size of the book (number of orders)."""
        return LIB.size(self._book)


# explicitly define the outward facing API of this module
__all__ = [LimitOrderBook.__name__]
