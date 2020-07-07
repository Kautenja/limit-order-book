"""A singleton instance of the C++ library to interact with."""


def load_library():
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


class Library:
    """The configured ctypes library."""
    # the functional API to interact with
    functions = load_library()


    def __init__(self):
        """Virtually private constructor."""
        raise RuntimeError("do not construct this class")


# explicitly define the outward facing API
__all__ = [Library.__name__]
