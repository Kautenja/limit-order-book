"""C++ declarations used by the Cython binding; widths match structures.hpp."""
from libc.stdint cimport uint32_t, uint64_t
from libcpp cimport bool as cpp_bool


cdef extern from "structures.hpp" namespace "LOB":
    ctypedef uint64_t UID
    ctypedef uint32_t Quantity
    ctypedef uint64_t Price
    ctypedef uint64_t Volume
    ctypedef uint32_t Count

    cdef enum class Side(cpp_bool):
        Sell
        Buy


cdef extern from "limit_order_book.hpp" namespace "LOB":
    cdef cppclass Book "LOB::LimitOrderBook":
        Book() except +
        void clear() except +
        void limit_sell(UID, Quantity, Price) except +
        void limit_buy(UID, Quantity, Price) except +
        void limit(Side, UID, Quantity, Price) except +
        cpp_bool has(UID) except +
        void cancel(UID) except +
        void market_sell(UID, Quantity) except +
        void market_buy(UID, Quantity) except +
        void market(Side, UID, Quantity) except +
        Price best_sell() except +
        Price best_buy() except +
        Price best(Side) except +
        Volume volume_sell() except +
        Volume volume_sell(Price) except +
        Volume volume_buy() except +
        Volume volume_buy(Price) except +
        Volume volume() except +
        Volume volume(Price) except +
        Count count_at(Price) except +
        Count count_sell() except +
        Count count_buy() except +
        Count count() except +
