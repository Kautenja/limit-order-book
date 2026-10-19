"""The Limit Order Book (LOB), backed by the Cython C++ extension."""
from ._native import NativeLimitOrderBook


class LimitOrderBook(NativeLimitOrderBook):
    """A price-time priority book with integer prices and quantities."""


# explicitly define the outward facing API of this module
__all__ = [LimitOrderBook.__name__]
