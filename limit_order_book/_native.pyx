"""The Limit Order Book (LOB)."""
from cpython.long cimport PyLong_AsUnsignedLongLongMask
from ._book cimport Book, Side, UID, Quantity, Price


cdef inline UID _uid(object value) except? -1:
    """Preserve ctypes unsigned 64-bit wrapping and integer-index conversion."""
    return PyLong_AsUnsignedLongLongMask(value)


cdef inline Quantity _quantity(object value) except? -1:
    """Preserve ctypes unsigned 32-bit wrapping, including on Windows."""
    return <Quantity>PyLong_AsUnsignedLongLongMask(value)


cdef inline Price _price(object value) except? -1:
    """Convert an integer tick price without losing its high bits."""
    return PyLong_AsUnsignedLongLongMask(value)


cdef class NativeLimitOrderBook:
    """The Limit Order Book (LOB)."""

    cdef Book* _book

    def __cinit__(self):
        """Allocate the book before Python initialization, including subclasses."""
        self._book = new Book()

    def __init__(self):
        """Initialize an empty book, also when explicitly initialized again."""
        self._book.clear()

    def __dealloc__(self):
        """Release all price levels and the owned C++ book exactly once."""
        if self._book != NULL:
            self._book.clear()
            del self._book

    def clear(self):
        """Clear all the orders in the book."""
        self._book.clear()

    def limit_sell(self, order_id, quantity, price):
        """
        Place a sell limit order with given quantity and price.

        Existing IDs are ignored without matching or changing the resting order.
        IDs can be reused after their orders are filled, cancelled, or cleared.

        Args:
            order_id: the ID for the order to add
            quantity: the quantity of shares in the limit order to place
            price: the price of the limit order to place

        Returns:
            None

        """
        self._book.limit_sell(_uid(order_id), _quantity(quantity), _price(price))

    def limit_buy(self, order_id, quantity, price):
        """
        Place a buy limit order with given quantity and price.

        Existing IDs are ignored without matching or changing the resting order.
        IDs can be reused after their orders are filled, cancelled, or cleared.

        Args:
            order_id: the ID for the order to add
            quantity: the quantity of shares in the limit order to place
            price: the price of the limit order to place

        Returns:
            None

        """
        self._book.limit_buy(_uid(order_id), _quantity(quantity), _price(price))

    def limit(self, side, order_id, quantity, price):
        """
        Place a limit order with given side, quantity, and price.

        Existing IDs are ignored without matching or changing the resting order.
        IDs can be reused after their orders are filled, cancelled, or cleared.

        Args:
            side: the side of the order to place
            order_id: the ID for the order to add
            quantity: the quantity of shares in the limit order to place
            price: the price of the limit order to place

        Returns:
            None

        """
        self._book.limit(
            <Side><bint>side, _uid(order_id), _quantity(quantity), _price(price)
        )

    def has(self, order_id):
        """
        Return true if the order with given ID is in the book, false otherwise.

        Args:
            order_id: the ID of the order to check for existence of

        Returns:
            True if the order is in the book, False otherwise

        """
        return self._book.has(_uid(order_id))

    def cancel(self, order_id):
        """
        Cancel an order with given order ID.

        Args:
            order_id: the ID of the order to cancel; unknown IDs are ignored

        Returns:
            None

        """
        cdef UID uid = _uid(order_id)
        if self._book.has(uid):
            self._book.cancel(uid)

    def market_sell(self, order_id, quantity):
        """
        Place a market sell order.

        Args:
            order_id: the ID for the order to add
            quantity: the quantity of shares in the market order to place

        Returns:
            None

        """
        self._book.market_sell(_uid(order_id), _quantity(quantity))

    def market_buy(self, order_id, quantity):
        """
        Place a market buy order.

        Args:
            order_id: the ID for the order to add
            quantity: the quantity of shares in the market order to place

        Returns:
            None

        """
        self._book.market_buy(_uid(order_id), _quantity(quantity))

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
        self._book.market(<Side><bint>side, _uid(order_id), _quantity(quantity))

    def best_sell(self):
        """Return the best sell price in the book."""
        return self._book.best_sell()

    def best_buy(self):
        """Return the best buy price in the book."""
        return self._book.best_buy()

    def best(self, side):
        """
        Return the best price for the given side.

        Args:
            side: the side of the book to get the best price for (False=sell)

        Returns:
            the best price for the given side of the book

        """
        return self._book.best(<Side><bint>side)

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
            return self._book.volume_sell()
        return self._book.volume_sell(_price(price))

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
            return self._book.volume_buy()
        return self._book.volume_buy(_price(price))

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
            return self._book.volume()
        return self._book.volume(_price(price))

    def count_at(self, price):
        """
        Return the count at the given limit price.

        Args:
            price: the price to get the count of the book for

        Returns:
            the count of the book at the given price

        """
        return self._book.count_at(_price(price))

    def count_sell(self):
        """Return the count of the book on the sell side."""
        return self._book.count_sell()

    def count_buy(self):
        """Return the count of the book on the buy side."""
        return self._book.count_buy()

    def count(self):
        """Return the total count of the book (number of orders)."""
        return self._book.count()
