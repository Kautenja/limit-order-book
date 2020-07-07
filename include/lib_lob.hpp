// A C level API to the framework.
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
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXTERNRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//

#ifndef LOB_LIB_LOB_HPP_
#define LOB_LIB_LOB_HPP_

#include "limit_order_book.hpp"

// Windows-base systems
#if defined(_WIN32)       || \
    defined(WIN32)        || \
    defined(__CYGWIN__)   || \
    defined(__MINGW32__)  || \
    defined(__BORLANDC__)
    // setup the module initializer. required to link visual studio C++ ctypes
    void PyInit_lib_limit_order_book() { }
    // setup the function modifier to export in the DLL
    #define EXTERN __declspec(dllexport)
// Unix-like systems
#else
    // setup the modifier as a dummy
    #define EXTERN
#endif

extern "C" {
    /// @brief Initialize a new limit order book and return a pointer to it.
    ///
    /// @returns a pointer to the new limit order book object
    ///
    EXTERN LOB::LimitOrderBook* new_() { return new LOB::LimitOrderBook(); }

    /// @brief Delete the limit order book instance, i.e., purge it from memory.
    ///
    /// @param book a pointer to the limit order book object
    ///
    EXTERN void delete_(LOB::LimitOrderBook* book) { delete book; }

    /// @brief Clear all the orders in the book.
    ///
    /// @param book a pointer to the limit order book object
    ///
    EXTERN void clear(LOB::LimitOrderBook* book) { book->clear(); }

    /// @brief Add a new sell limit order to the book.
    ///
    /// @param book a pointer to the limit order book object
    /// @param uid the ID for the order
    /// @param quantity the number of shares to sell
    /// @param price the limit price for the order
    ///
    EXTERN void limit_sell(
        LOB::LimitOrderBook* book,
        LOB::UID uid,
        LOB::Quantity quantity,
        LOB::Price price
    ) { book->limit_sell(uid, quantity, price); }

    /// @brief Add a new buy limit order to the book.
    ///
    /// @param book a pointer to the limit order book object
    /// @param uid the ID for the order
    /// @param quantity the number of shares to buy
    /// @param price the limit price for the order
    ///
    EXTERN void limit_buy(
        LOB::LimitOrderBook* book,
        LOB::UID uid,
        LOB::Quantity quantity,
        LOB::Price price
    ) { book->limit_buy(uid, quantity, price); }

    /// @brief Add a new limit order to the book.
    ///
    /// @param book a pointer to the limit order book object
    /// @param side whether the order is a buy (true) or sell (false)
    /// @param uid the ID for the order
    /// @param quantity the number of shares to sell
    /// @param price the limit price for the order
    ///
    EXTERN void limit(
        LOB::LimitOrderBook* book,
        LOB::Side side,
        LOB::UID uid,
        LOB::Quantity quantity,
        LOB::Price price
    ) { book->limit(side, uid, quantity, price); }

    /// @brief Return true if the book has an order with given ID.
    ///
    /// @param book a pointer to the limit order book object
    /// @param uid the order ID of the order to check for existence of
    /// @returns true if the book has an order with given ID, false otherwise
    ///
    EXTERN bool has(LOB::LimitOrderBook* book, LOB::UID uid) {
        return book->has(uid);
    }

    /// @brief Get the order with given ID.
    ///
    /// @param book a pointer to the limit order book object
    /// @param uid the order ID of the order to get
    /// @returns a pointer to the order with given order ID
    ///
    EXTERN const LOB::Order* get(LOB::LimitOrderBook* book, LOB::UID uid) {
        return &book->get(uid);
    }

    /// @brief Cancel an existing order in the book.
    ///
    /// @param book a pointer to the limit order book object
    /// @param order_id the ID of the order to cancel
    ///
    EXTERN void cancel(LOB::LimitOrderBook* book, LOB::UID uid) {
        book->cancel(uid);
    }

    /// @brief Execute a sell market order.
    ///
    /// @param book a pointer to the limit order book object
    /// @param uid the ID for the order
    /// @param quantity the quantity in the market order
    ///
    EXTERN void market_sell(
        LOB::LimitOrderBook* book,
        LOB::UID uid,
        LOB::Quantity quantity
    ) { book->market_sell(uid, quantity); }

    /// @brief Execute a buy market order.
    ///
    /// @param book a pointer to the limit order book object
    /// @param uid the ID for the order
    /// @param quantity the quantity in the market order
    ///
    EXTERN void market_buy(
        LOB::LimitOrderBook* book,
        LOB::UID uid,
        LOB::Quantity quantity
    ) { book->market_buy(uid, quantity); }

    /// @brief Execute a market order.
    ///
    /// @param book a pointer to the limit order book object
    /// @param side whether the order is a buy (true) or sell (false)
    /// @param uid the ID for the order
    /// @param quantity the quantity in the market order
    ///
    EXTERN void market(
        LOB::LimitOrderBook* book,
        LOB::Side side,
        LOB::UID uid,
        LOB::Quantity quantity
    ) { book->market(side, uid, quantity); }

    /// @brief Return the best sell price.
    ///
    /// @param book a pointer to the limit order book object
    /// @returns the best ask price in the book
    ///
    EXTERN LOB::Price best_sell(LOB::LimitOrderBook* book) {
        return book->best_sell();
    }

    /// @brief Return the best buy price.
    ///
    /// @param book a pointer to the limit order book object
    /// @returns the best bid price in the book
    ///
    EXTERN LOB::Price best_buy(LOB::LimitOrderBook* book) {
        return book->best_buy();
    }

    /// @brief Return the best price for the given side.
    ///
    /// @param book a pointer to the limit order book object
    /// @param side the side to get the best price from
    /// @returns the best price in the book for the given side
    ///
    EXTERN LOB::Price best(LOB::LimitOrderBook* book, LOB::Side side) {
        return book->best(side);
    }

    /// @brief Return the total volume for the sell side of the book.
    ///
    /// @param book a pointer to the limit order book object
    /// @param price the limit price to get the volume for
    /// @return the volume for the given limit price
    ///
    EXTERN LOB::Volume volume_sell_price(
        LOB::LimitOrderBook* book,
        LOB::Price price
    ) { return book->volume_sell(price); }

    /// @brief Return the total volume for the sell side of the book.
    ///
    /// @param book a pointer to the limit order book object
    /// @return the total volume on the sell side of the book
    ///
    EXTERN LOB::Volume volume_sell(LOB::LimitOrderBook* book) {
        return book->volume_sell();
    }

    /// @brief Return the total volume for the buy side of the book.
    ///
    /// @param book a pointer to the limit order book object
    /// @param price the limit price to get the volume for
    /// @return the volume for the given limit price
    ///
    EXTERN LOB::Volume volume_buy_price(
        LOB::LimitOrderBook* book,
        LOB::Price price
    ) { return book->volume_buy(price); }

    /// @brief Return the total volume for the buy side of the book.
    ///
    /// @param book a pointer to the limit order book object
    /// @return the total volume on the buy side of the book
    ///
    EXTERN LOB::Volume volume_buy(LOB::LimitOrderBook* book) {
        return book->volume_buy();
    }

    /// @brief Return the volume at the given limit price.
    ///
    /// @param book a pointer to the limit order book object
    /// @returns the volume of orders at the given limit price
    ///
    EXTERN LOB::Volume volume_price(
        LOB::LimitOrderBook* book,
        LOB::Price price
    ) { return book->volume(price); }

    /// @brief Return the total volume for the book.
    ///
    /// @param book a pointer to the limit order book object
    /// @returns the total volume of orders in the book
    ///
    EXTERN LOB::Volume volume(LOB::LimitOrderBook* book) {
        return book->volume();
    }

    /// @brief Return the order count at the given limit price.
    ///
    /// @param book a pointer to the limit order book object
    /// @param price the price to get the order count at
    /// @returns the number of orders at the given price
    ///
    EXTERN LOB::Count count_at(LOB::LimitOrderBook* book, LOB::Price price) {
        return book->count_at(price);
    }

    /// @brief Return the order count of the sell side of the book.
    ///
    /// @param book a pointer to the limit order book object
    /// @returns the number of orders on the sell side of the book
    ///
    EXTERN LOB::Count count_sell(LOB::LimitOrderBook* book) {
        return book->count_sell();
    }

    /// @brief Return the order count of the buy side of the book.
    ///
    /// @param book a pointer to the limit order book object
    /// @returns the number of orders on the buy side of the book
    ///
    EXTERN LOB::Count count_buy(LOB::LimitOrderBook* book) {
        return book->count_buy();
    }

    /// @brief Return the order count of the book.
    ///
    /// @param book a pointer to the limit order book object
    /// @returns the number of orders in the book
    ///
    EXTERN LOB::Count count(LOB::LimitOrderBook* book) {
        return book->count();
    }
}

// un-define the export macro
#undef EXTERN

#endif  // LOB_LIB_LOB_HPP_
