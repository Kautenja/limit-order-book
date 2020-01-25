"""The Limit Order Book (LOB)."""
import ctypes
import os
import glob


LIB_PATH = os.path.join(os.path.dirname(__file__), 'lib_limit_order_book*')
try:  # load the library from the shared object file
    LIB = ctypes.cdll.LoadLibrary(glob.glob(LIB_PATH)[0])
except IndexError:
    raise OSError('missing static lib_limit_order_book*.so library!')


class Types:
    """Types used in the C++ code."""
    # a type for pointers
    Pointer = ctypes.c_void_p
    # a type for the side of the order (i.e., sell or buy)
    Side = ctypes.c_bool
    # a type for the unique order ID
    UID = ctypes.c_uint64
    # a type for the size of an order
    Size = ctypes.c_uint32
    # a type for the price of an order
    Price = ctypes.c_uint64
    # a type for the volume measures in the book
    Volume = ctypes.c_uint32


# setup the argument and return types for new_
LIB.new_.argtypes = None
LIB.new_.restype = Types.Pointer
# setup the argument and return types for delete_
LIB.delete_.argtypes = [Types.Pointer]
LIB.delete_.restype = None
# setup the argument and return types for clear
LIB.clear.argtypes = [Types.Pointer]
LIB.clear.restype = None


# setup the argument and return types for limit_sell
LIB.limit_sell.argtypes = [Types.Pointer, Types.UID, Types.Size, Types.Price]
LIB.limit_sell.restype = None
# setup the argument and return types for limit_buy
LIB.limit_buy.argtypes = [Types.Pointer, Types.UID, Types.Size, Types.Price]
LIB.limit_buy.restype = None
# setup the argument and return types for limit
LIB.limit.argtypes = [Types.Pointer, Types.Side, Types.UID, Types.Size, Types.Price]
LIB.limit.restype = None


# setup the argument and return types for has
LIB.has.argtypes = [Types.Pointer, Types.UID]
LIB.has.restype = ctypes.c_bool
# setup the argument and return types for get
LIB.get.argtypes = [Types.Pointer, Types.UID]
LIB.get.restype = Types.Pointer
# setup the argument and return types for cancel
LIB.cancel.argtypes = [Types.Pointer, Types.UID]
LIB.cancel.restype = None


# setup the argument and return types for market_sell
LIB.market_sell.argtypes = [Types.Pointer, Types.UID, Types.Size]
LIB.market_sell.restype = None
# setup the argument and return types for market_buy
LIB.market_buy.argtypes = [Types.Pointer, Types.UID, Types.Size]
LIB.market_buy.restype = None
# setup the argument and return types for market
LIB.market.argtypes = [Types.Pointer, Types.Side, Types.UID, Types.Size]
LIB.market.restype = None


# setup the argument and return types for best_sell
LIB.best_sell.argtypes = [Types.Pointer]
LIB.best_sell.restype = Types.Price
# setup the argument and return types for best_buy
LIB.best_buy.argtypes = [Types.Pointer]
LIB.best_buy.restype = Types.Price
# setup the argument and return types for best
LIB.best.argtypes = [Types.Pointer, Types.Side]
LIB.best.restype = Types.Price


# setup the argument and return types for volume_sell_price
LIB.volume_sell_price.argtypes = [Types.Pointer, Types.Price]
LIB.volume_sell_price.restype = Types.Volume
# setup the argument and return types for volume_sell
LIB.volume_sell.argtypes = [Types.Pointer]
LIB.volume_sell.restype = Types.Volume
# setup the argument and return types for volume_buy_price
LIB.volume_buy_price.argtypes = [Types.Pointer, Types.Price]
LIB.volume_buy_price.restype = Types.Volume
# setup the argument and return types for volume_buy
LIB.volume_buy.argtypes = [Types.Pointer]
LIB.volume_buy.restype = Types.Volume
# setup the argument and return types for volume_price
LIB.volume_price.argtypes = [Types.Pointer, Types.Price]
LIB.volume_price.restype = Types.Volume
# setup the argument and return types for volume
LIB.volume.argtypes = [Types.Pointer]
LIB.volume.restype = Types.Volume


# setup the argument and return types for size_at
LIB.size_at.argtypes = [Types.Pointer, Types.Price]
LIB.size_at.restype = Types.Size
# setup the argument and return types for size_sell
LIB.size_sell.argtypes = [Types.Pointer]
LIB.size_sell.restype = Types.Size
# setup the argument and return types for size_buy
LIB.size_buy.argtypes = [Types.Pointer]
LIB.size_buy.restype = Types.Size
# setup the argument and return types for size
LIB.size.argtypes = [Types.Pointer]
LIB.size.restype = Types.Size


class LimitOrderBook:
    """The Limit Order Book (LOB)."""

    def __init__(self):
        """Initialize a new limit order book."""
        self._book = LIB.new_()

    def __del__(self):
        """Delete this limit order book."""
        LIB.delete_(self._book)

    def clear(self):
        """Clear all the orders in the book."""
        LIB.clear(self._book)

    def limit_sell(self, order_id, size, price):
        """
        Place a sell limit order with given size and price.

        Args:
            order_id: the ID for the order to add
            size: the size of the limit order to place
            price: the price of the limit order to place

        Returns:
            None

        """
        LIB.limit_sell(self._book, order_id, size, price)

    def limit_buy(self, order_id, size, price):
        """
        Place a sell limit order with given size and price.

        Args:
            order_id: the ID for the order to add
            size: the size of the limit order to place
            price: the price of the limit order to place

        Returns:
            None

        """
        LIB.limit_buy(self._book, order_id, size, price)

    def limit(self, side, order_id, size, price):
        """
        Place a sell limit order with given size and price.

        Args:
            side: the side of the order to place
            order_id: the ID for the order to add
            size: the size of the limit order to place
            price: the price of the limit order to place

        Returns:
            None

        """
        LIB.limit_buy(self._book, side, order_id, size, price)

    def has(self, order_id):
        """
        Return true if the order with given ID is in the book, false otherwise.

        Args:
            order_id: the ID of the order to check for existence of

        Returns:
            True if the order is in the book, False otherwise

        """
        return LIB.has(self._book, order_id)

    def get(self, order_id):
        """
        Return a pointer to the order with given ID.

        Args:
            order_id: the ID of the order to get

        Returns:
            an order object wrapped around the pointer

        """
        # pointer = LIB.get(self._book, order_id)
        raise NotImplementedError

    def cancel(self, order_id):
        """
        Cancel an order with given order ID.

        Args:
            order_id: the ID of the order to cancel

        Returns:
            None

        """
        LIB.cancel(self._book, order_id)

    def market_sell(self, order_id, size):
        """
        Place a market sell order.

        Args:
            order_id: the ID for the order to add
            size: the size of the market order to place

        Returns:
            None

        """
        LIB.market_sell(self._book, order_id, size)

    def market_buy(self, order_id, size):
        """
        Place a market buy order.

        Args:
            order_id: the ID for the order to add
            size: the size of the market order to place

        Returns:
            None

        """
        LIB.market_buy(self._book, order_id, size)

    def market(self, side, order_id, size):
        """
        Place a market order.

        Args:
            side: the side of the order to place
            order_id: the ID for the order to add
            size: the size of the market order to place

        Returns:
            None

        """
        LIB.market(self._book, side, order_id, size)

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
