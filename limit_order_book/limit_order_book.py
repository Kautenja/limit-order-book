"""The Limit Order Book (LOB)."""
from .library import Library


class LimitOrderBook:
    """The Limit Order Book (LOB)."""

    def __init__(self):
        """Initialize a new limit order book."""
        self._book = Library.functions.new_()

    def __del__(self):
        """Delete this limit order book."""
        Library.functions.delete_(self._book)

    def clear(self):
        """Clear all the orders in the book."""
        Library.functions.clear(self._book)

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
        Library.functions.limit_sell(self._book, order_id, quantity, price)

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
        Library.functions.limit_buy(self._book, order_id, quantity, price)

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
        Library.functions.limit(self._book, side, order_id, quantity, price)

    def has(self, order_id):
        """
        Return true if the order with given ID is in the book, false otherwise.

        Args:
            order_id: the ID of the order to check for existence of

        Returns:
            True if the order is in the book, False otherwise

        """
        return Library.functions.has(self._book, order_id)

    # def get(self, order_id):
    #     """
    #     Return a pointer to the order with given ID.
    #
    #     Args:
    #         order_id: the ID of the order to get
    #
    #     Returns:
    #         an order object wrapped around the pointer
    #
    #     """
    #     # pointer = Library.functions.get(self._book, order_id)
    #     raise NotImplementedError

    def cancel(self, order_id):
        """
        Cancel an order with given order ID.

        Args:
            order_id: the ID of the order to cancel

        Returns:
            None

        """
        Library.functions.cancel(self._book, order_id)

    def market_sell(self, order_id, quantity):
        """
        Place a market sell order.

        Args:
            order_id: the ID for the order to add
            quantity: the quantity of shares in the market order to place

        Returns:
            None

        """
        Library.functions.market_sell(self._book, order_id, quantity)

    def market_buy(self, order_id, quantity):
        """
        Place a market buy order.

        Args:
            order_id: the ID for the order to add
            quantity: the quantity of shares in the market order to place

        Returns:
            None

        """
        Library.functions.market_buy(self._book, order_id, quantity)

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
        Library.functions.market(self._book, side, order_id, quantity)

    def best_sell(self):
        """Return the best sell price in the book."""
        return Library.functions.best_sell(self._book)

    def best_buy(self):
        """Return the best buy price in the book."""
        return Library.functions.best_buy(self._book)

    def best(self, side):
        """
        Return the best price for the given side.

        Args:
            side: the side of the book to get the best price for (False=sell)

        Returns:
            the best price for the given side of the book

        """
        return Library.functions.best(self._book, side)

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
            return Library.functions.volume_sell(self._book)
        return Library.functions.volume_sell_price(self._book, price)

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
            return Library.functions.volume_buy(self._book)
        return Library.functions.volume_buy_price(self._book, price)

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
            return Library.functions.volume(self._book)
        return Library.functions.volume_price(self._book, price)

    def count_at(self, price):
        """
        Return the count at the given limit price.

        Args:
            price: the price to get the count of the book for

        Returns:
            the count of the book at the given price

        """
        return Library.functions.count_at(self._book, price)

    def count_sell(self):
        """Return the count of the book on the sell side."""
        return Library.functions.count_sell(self._book)

    def count_buy(self):
        """Return the count of the book on the buy side."""
        return Library.functions.count_buy(self._book)

    def count(self):
        """Return the total count of the book (number of orders)."""
        return Library.functions.count(self._book)


# explicitly define the outward facing API of this module
__all__ = [LimitOrderBook.__name__]
