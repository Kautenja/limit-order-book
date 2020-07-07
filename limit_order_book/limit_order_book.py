"""The Limit Order Book (LOB)."""


def _load_library():
    """
    Load the ctypes library and configure the parameter and return types.

    Returns:
        the ctypes library object

    """
    import ctypes
    import os
    import glob
    # attempt to load the binary library from file
    libpath = os.path.join(os.path.dirname(__file__), 'lib_limit_order_book*')
    try:  # load the library from the shared object file
        library = ctypes.cdll.LoadLibrary(glob.glob(libpath)[0])
    except IndexError:
        raise OSError('missing static lib_limit_order_book*.so library!')

    # a type for pointers
    Pointer = ctypes.c_void_p
    # a type for the side of the order (i.e., sell or buy)
    Side = ctypes.c_bool
    # a type for the unique order ID
    UID = ctypes.c_uint64
    # a type for the quantity of an order
    Quantity = ctypes.c_uint32
    # a type for the price of an order
    Price = ctypes.c_uint64
    # a type for the volume measures in the book
    Volume = ctypes.c_uint32
    # a type for counting orders (accumulator)
    Count = ctypes.c_uint32

    # setup the argument and return types for new_
    library.new_.argtypes = None
    library.new_.restype = Pointer
    # setup the argument and return types for delete_
    library.delete_.argtypes = [Pointer]
    library.delete_.restype = None
    # setup the argument and return types for clear
    library.clear.argtypes = [Pointer]
    library.clear.restype = None

    # setup the argument and return types for limit_sell
    library.limit_sell.argtypes = [Pointer, UID, Quantity, Price]
    library.limit_sell.restype = None
    # setup the argument and return types for limit_buy
    library.limit_buy.argtypes = [Pointer, UID, Quantity, Price]
    library.limit_buy.restype = None
    # setup the argument and return types for limit
    library.limit.argtypes = [Pointer, Side, UID, Quantity, Price]
    library.limit.restype = None

    # setup the argument and return types for has
    library.has.argtypes = [Pointer, UID]
    library.has.restype = ctypes.c_bool
    # setup the argument and return types for get
    library.get.argtypes = [Pointer, UID]
    library.get.restype = Pointer
    # setup the argument and return types for cancel
    library.cancel.argtypes = [Pointer, UID]
    library.cancel.restype = None

    # setup the argument and return types for market_sell
    library.market_sell.argtypes = [Pointer, UID, Quantity]
    library.market_sell.restype = None
    # setup the argument and return types for market_buy
    library.market_buy.argtypes = [Pointer, UID, Quantity]
    library.market_buy.restype = None
    # setup the argument and return types for market
    library.market.argtypes = [Pointer, Side, UID, Quantity]
    library.market.restype = None

    # setup the argument and return types for best_sell
    library.best_sell.argtypes = [Pointer]
    library.best_sell.restype = Price
    # setup the argument and return types for best_buy
    library.best_buy.argtypes = [Pointer]
    library.best_buy.restype = Price
    # setup the argument and return types for best
    library.best.argtypes = [Pointer, Side]
    library.best.restype = Price

    # setup the argument and return types for volume_sell_price
    library.volume_sell_price.argtypes = [Pointer, Price]
    library.volume_sell_price.restype = Volume
    # setup the argument and return types for volume_sell
    library.volume_sell.argtypes = [Pointer]
    library.volume_sell.restype = Volume
    # setup the argument and return types for volume_buy_price
    library.volume_buy_price.argtypes = [Pointer, Price]
    library.volume_buy_price.restype = Volume
    # setup the argument and return types for volume_buy
    library.volume_buy.argtypes = [Pointer]
    library.volume_buy.restype = Volume
    # setup the argument and return types for volume_price
    library.volume_price.argtypes = [Pointer, Price]
    library.volume_price.restype = Volume
    # setup the argument and return types for volume
    library.volume.argtypes = [Pointer]
    library.volume.restype = Volume

    # setup the argument and return types for count_at
    library.count_at.argtypes = [Pointer, Price]
    library.count_at.restype = Count
    # setup the argument and return types for count_sell
    library.count_sell.argtypes = [Pointer]
    library.count_sell.restype = Count
    # setup the argument and return types for count_buy
    library.count_buy.argtypes = [Pointer]
    library.count_buy.restype = Count
    # setup the argument and return types for count
    library.count.argtypes = [Pointer]
    library.count.restype = Count

    return library


class LimitOrderBook:
    """The Limit Order Book (LOB)."""

    # the class instance of the ctypes-based library to interact with
    _library = _load_library()

    def __init__(self):
        """Initialize a new limit order book."""
        self._book = self._library.new_()

    def __del__(self):
        """Delete this limit order book."""
        self._library.delete_(self._book)

    def clear(self):
        """Clear all the orders in the book."""
        self._library.clear(self._book)

    def limit_sell(self, order_id, quantity, price):
        """
        Place a sell limit order with given quantity and price.

        Args:
            order_id: the ID for the order to add
            quantity: the quantity of shares in the limit order to place
            price: the price of the limit order to place

        Returns:
            None

        """
        self._library.limit_sell(self._book, order_id, quantity, price)

    def limit_buy(self, order_id, quantity, price):
        """
        Place a sell limit order with given quantity and price.

        Args:
            order_id: the ID for the order to add
            quantity: the quantity of shares in the limit order to place
            price: the price of the limit order to place

        Returns:
            None

        """
        self._library.limit_buy(self._book, order_id, quantity, price)

    def limit(self, side, order_id, quantity, price):
        """
        Place a sell limit order with given quantity and price.

        Args:
            side: the side of the order to place
            order_id: the ID for the order to add
            quantity: the quantity of shares in the limit order to place
            price: the price of the limit order to place

        Returns:
            None

        """
        self._library.limit(self._book, side, order_id, quantity, price)

    def has(self, order_id):
        """
        Return true if the order with given ID is in the book, false otherwise.

        Args:
            order_id: the ID of the order to check for existence of

        Returns:
            True if the order is in the book, False otherwise

        """
        return self._library.has(self._book, order_id)

    def get(self, order_id):
        """
        Return a pointer to the order with given ID.

        Args:
            order_id: the ID of the order to get

        Returns:
            an order object wrapped around the pointer

        """
        # pointer = self._library.get(self._book, order_id)
        raise NotImplementedError

    def cancel(self, order_id):
        """
        Cancel an order with given order ID.

        Args:
            order_id: the ID of the order to cancel

        Returns:
            None

        """
        self._library.cancel(self._book, order_id)

    def market_sell(self, order_id, quantity):
        """
        Place a market sell order.

        Args:
            order_id: the ID for the order to add
            quantity: the quantity of shares in the market order to place

        Returns:
            None

        """
        self._library.market_sell(self._book, order_id, quantity)

    def market_buy(self, order_id, quantity):
        """
        Place a market buy order.

        Args:
            order_id: the ID for the order to add
            quantity: the quantity of shares in the market order to place

        Returns:
            None

        """
        self._library.market_buy(self._book, order_id, quantity)

    def market(self, side, order_id, quantity):
        """
        Place a market order.

        Args:
            side: the side of the order to place
            order_id: the ID for the order to add
            quantity: the quantity of shares in the market order to place

        Returns:
            None

        """
        self._library.market(self._book, side, order_id, quantity)

    def best_sell(self):
        """Return the best sell price in the book."""
        return self._library.best_sell(self._book)

    def best_buy(self):
        """Return the best buy price in the book."""
        return self._library.best_buy(self._book)

    def best(self, side):
        """
        Return the best price for the given side.

        Args:
            side: the side of the book to get the best price for (False=sell)

        Returns:
            the best price for the given side of the book

        """
        return self._library.best(self._book, side)

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
            return self._library.volume_sell(self._book)
        return self._library.volume_sell_price(self._book, price)

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
            return self._library.volume_buy(self._book)
        return self._library.volume_buy_price(self._book, price)

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
            return self._library.volume(self._book)
        return self._library.volume_price(self._book, price)

    def count_at(self, price):
        """
        Return the count at the given limit price.

        Args:
            price: the price to get the count of the book for

        Returns:
            the count of the book at the given price

        """
        return self._library.count_at(self._book, price)

    def count_sell(self):
        """Return the count of the book on the sell side."""
        return self._library.count_sell(self._book)

    def count_buy(self):
        """Return the count of the book on the buy side."""
        return self._library.count_buy(self._book)

    def count(self):
        """Return the total count of the book (number of orders)."""
        return self._library.count(self._book)


# explicitly define the outward facing API of this module
__all__ = [LimitOrderBook.__name__]
