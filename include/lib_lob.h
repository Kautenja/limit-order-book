// A C API to the limit order book for C programs and Python's ctypes.
//
// Copyright (c) 2020 Christian Kauten
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//

#ifndef LOB_LIB_LOB_H_
#define LOB_LIB_LOB_H_

// This header is valid C99 and C++14. It declares the functions of the
// limit_order_book shared library (the limit_order_book::c CMake target);
// src/lib_lob.cpp defines them.

#include <stdbool.h>
#include <stdint.h>

// LOB_C_API marks a function exported from the shared library. Windows needs
// dllexport while building the library and dllimport while using it.
#if defined(_WIN32) || defined(__CYGWIN__)
    #if defined(LOB_C_BUILDING_LIBRARY)
        #define LOB_C_API __declspec(dllexport)
    #else
        #define LOB_C_API __declspec(dllimport)
    #endif
#elif defined(__GNUC__)
    #define LOB_C_API __attribute__((visibility("default")))
#else
    #define LOB_C_API
#endif

#ifdef __cplusplus
extern "C" {
#endif

/// @brief An opaque handle to a limit order book, owned by the library.
typedef struct LOB_LimitOrderBook LOB_LimitOrderBook;
/// @brief An opaque handle to an order resting in a limit order book.
typedef struct LOB_Order LOB_Order;
/// @brief The side of an order: true for buy and false for sell.
typedef bool LOB_Side;
/// @brief A unique identifier for an order.
typedef uint64_t LOB_UID;
/// @brief A number of shares in an order.
typedef uint32_t LOB_Quantity;
/// @brief A limit price in integer ticks with no implied scale.
typedef uint64_t LOB_Price;
/// @brief A total number of shares across orders.
typedef uint64_t LOB_Volume;
/// @brief A number of orders.
typedef uint32_t LOB_Count;

/// @brief Create a new, empty limit order book.
///
/// @returns an owning handle to the new book; release it with `delete_`
///
LOB_C_API LOB_LimitOrderBook* new_(void);

/// @brief Delete a limit order book and every order in it.
///
/// @param book the book to delete; it is invalid after the call
///
LOB_C_API void delete_(LOB_LimitOrderBook* book);

/// @brief Remove every order from the book.
///
/// @param book the book to clear
///
LOB_C_API void clear(LOB_LimitOrderBook* book);

/// @brief Add a sell limit order, which first executes against the buy side.
///
/// Existing IDs are ignored without matching or changing the resting order.
/// An ID can be reused after its order is filled, cancelled, or cleared.
///
/// @param book the book to add the order to
/// @param uid the ID for the order
/// @param quantity the number of shares to sell
/// @param price the limit price for the order
///
LOB_C_API void limit_sell(
    LOB_LimitOrderBook* book,
    LOB_UID uid,
    LOB_Quantity quantity,
    LOB_Price price
);

/// @brief Add a buy limit order, which first executes against the sell side.
///
/// Existing IDs are ignored without matching or changing the resting order.
/// An ID can be reused after its order is filled, cancelled, or cleared.
///
/// @param book the book to add the order to
/// @param uid the ID for the order
/// @param quantity the number of shares to buy
/// @param price the limit price for the order
///
LOB_C_API void limit_buy(
    LOB_LimitOrderBook* book,
    LOB_UID uid,
    LOB_Quantity quantity,
    LOB_Price price
);

/// @brief Add a limit order to the given side of the book.
///
/// Existing IDs are ignored without matching or changing the resting order.
/// An ID can be reused after its order is filled, cancelled, or cleared.
///
/// @param book the book to add the order to
/// @param side whether the order is a buy (true) or sell (false)
/// @param uid the ID for the order
/// @param quantity the number of shares to buy or sell
/// @param price the limit price for the order
///
LOB_C_API void limit(
    LOB_LimitOrderBook* book,
    LOB_Side side,
    LOB_UID uid,
    LOB_Quantity quantity,
    LOB_Price price
);

/// @brief Return true if the book has a resting order with the given ID.
///
/// @param book the book to search
/// @param uid the ID of the order to look for
/// @returns true if the order is in the book, false otherwise
///
LOB_C_API bool has(LOB_LimitOrderBook* book, LOB_UID uid);

/// @brief Return the resting order with the given ID.
///
/// @param book the book to search
/// @param uid the ID of the order to get
/// @returns a borrowed handle to the order, or NULL if the book has no order
/// with the ID. The handle is invalid once the order fills, is cancelled, or
/// the book is cleared or deleted.
///
LOB_C_API const LOB_Order* get(LOB_LimitOrderBook* book, LOB_UID uid);

/// @brief Cancel a resting order. Unknown IDs are ignored.
///
/// @param book the book to cancel the order in
/// @param uid the ID of the order to cancel
///
LOB_C_API void cancel(LOB_LimitOrderBook* book, LOB_UID uid);

/// @brief Execute a sell market order against the buy side.
///
/// @param book the book to execute the order in
/// @param uid the ID for the order
/// @param quantity the number of shares to sell
///
LOB_C_API void market_sell(
    LOB_LimitOrderBook* book,
    LOB_UID uid,
    LOB_Quantity quantity
);

/// @brief Execute a buy market order against the sell side.
///
/// @param book the book to execute the order in
/// @param uid the ID for the order
/// @param quantity the number of shares to buy
///
LOB_C_API void market_buy(
    LOB_LimitOrderBook* book,
    LOB_UID uid,
    LOB_Quantity quantity
);

/// @brief Execute a market order against the opposite side of the book.
///
/// @param book the book to execute the order in
/// @param side whether the order is a buy (true) or sell (false)
/// @param uid the ID for the order
/// @param quantity the number of shares to buy or sell
///
LOB_C_API void market(
    LOB_LimitOrderBook* book,
    LOB_Side side,
    LOB_UID uid,
    LOB_Quantity quantity
);

/// @brief Return the best sell price.
///
/// @param book the book to query
/// @returns the lowest sell limit price, or 0 if the sell side is empty
///
LOB_C_API LOB_Price best_sell(LOB_LimitOrderBook* book);

/// @brief Return the best buy price.
///
/// @param book the book to query
/// @returns the highest buy limit price, or 0 if the buy side is empty
///
LOB_C_API LOB_Price best_buy(LOB_LimitOrderBook* book);

/// @brief Return the best price for the given side.
///
/// @param book the book to query
/// @param side the side to query: buy (true) or sell (false)
/// @returns the best limit price on the side, or 0 if the side is empty
///
LOB_C_API LOB_Price best(LOB_LimitOrderBook* book, LOB_Side side);

/// @brief Return the sell volume at the given limit price.
///
/// @param book the book to query
/// @param price the limit price to get the volume for
/// @returns the number of shares for sale at the price
///
LOB_C_API LOB_Volume volume_sell_price(
    LOB_LimitOrderBook* book,
    LOB_Price price
);

/// @brief Return the total volume on the sell side of the book.
///
/// @param book the book to query
/// @returns the number of shares for sale across every sell limit
///
LOB_C_API LOB_Volume volume_sell(LOB_LimitOrderBook* book);

/// @brief Return the buy volume at the given limit price.
///
/// @param book the book to query
/// @param price the limit price to get the volume for
/// @returns the number of shares bid for at the price
///
LOB_C_API LOB_Volume volume_buy_price(
    LOB_LimitOrderBook* book,
    LOB_Price price
);

/// @brief Return the total volume on the buy side of the book.
///
/// @param book the book to query
/// @returns the number of shares bid for across every buy limit
///
LOB_C_API LOB_Volume volume_buy(LOB_LimitOrderBook* book);

/// @brief Return the volume at the given limit price on either side.
///
/// @param book the book to query
/// @param price the limit price to get the volume for
/// @returns the number of shares resting at the price
///
LOB_C_API LOB_Volume volume_price(LOB_LimitOrderBook* book, LOB_Price price);

/// @brief Return the total volume in the book.
///
/// @param book the book to query
/// @returns the number of shares resting on both sides
///
LOB_C_API LOB_Volume volume(LOB_LimitOrderBook* book);

/// @brief Return the number of orders at the given limit price.
///
/// @param book the book to query
/// @param price the limit price to count orders at
/// @returns the number of orders resting at the price
///
LOB_C_API LOB_Count count_at(LOB_LimitOrderBook* book, LOB_Price price);

/// @brief Return the number of orders on the sell side of the book.
///
/// @param book the book to query
/// @returns the number of resting sell orders
///
LOB_C_API LOB_Count count_sell(LOB_LimitOrderBook* book);

/// @brief Return the number of orders on the buy side of the book.
///
/// @param book the book to query
/// @returns the number of resting buy orders
///
LOB_C_API LOB_Count count_buy(LOB_LimitOrderBook* book);

/// @brief Return the number of orders in the book.
///
/// @param book the book to query
/// @returns the number of resting orders on both sides
///
LOB_C_API LOB_Count count(LOB_LimitOrderBook* book);

#ifdef __cplusplus
}  // extern "C"
#endif

// un-define the export macro
#undef LOB_C_API

#endif  // LOB_LIB_LOB_H_
