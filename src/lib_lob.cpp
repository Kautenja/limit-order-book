//  Program:      lob
//  File:         lib_lob.cpp
//  Description:  file describes the outward facing ctypes API for Python
//
//  Copyright (c) 2019 Christian Kauten. All rights reserved.
//

#include <string>
#include "limit_order_book.hpp"

// Windows-base systems
#if defined(_WIN32) || defined(WIN32) || defined(__CYGWIN__) || defined(__MINGW32__) || defined(__BORLANDC__)
    // setup the module initializer. required to link visual studio C++ ctypes
    void PyInit_lib_limit_order_book() { }
    // setup the function modifier to export in the DLL
    #define EXP __declspec(dllexport)
// Unix-like systems
#else
    // setup the modifier as a dummy
    #define EXP
#endif

using namespace LOB;

// definitions of functions for the Python interface to access
extern "C" {
    /// Initialize a new limit order book and return a pointer to it
    EXP LimitOrderBook* new_() {
        return new LimitOrderBook();
    }

    /// Delete the limit order book, i.e., purge it from memory
    EXP void delete_(LimitOrderBook* book) {
        delete book;
    }

    /// Clear all the orders in the book.
    EXP void clear(LimitOrderBook* book) {
        book->clear();
    }

    /// Add a new sell limit order to the book.
    EXP void limit_sell(LimitOrderBook* book, UID order_id, Size size, Price price) {
        book->limit_sell(order_id, size, price, 0);
    }

    /// Add a new buy limit order to the book.
    EXP void limit_buy(LimitOrderBook* book, UID order_id, Size size, Price price) {
        book->limit_buy(order_id, size, price, 0);
    }

    /// Add a new limit order to the book.
    EXP void limit(LimitOrderBook* book, Side side, UID order_id, Size size, Price price) {
        book->limit(side, order_id, size, price, 0);
    }

    /// Return true if the book has an order with given ID, false otherwise.
    EXP bool has(LimitOrderBook* book, UID order_id) {
        return book->has(order_id);
    }

    /// Get the order with given ID.
    EXP const Order* get(LimitOrderBook* book, UID order_id) {
        return &book->get(order_id);
    }

    /// Cancel an existing order in the book.
    EXP void cancel(LimitOrderBook* book, UID order_id) {
        book->cancel(order_id);
    }

    /// Execute a sell market order.
    EXP void market_sell(LimitOrderBook* book, UID order_id, Size size) {
        book->market_sell(order_id, size, 0);
    }

    /// Execute a buy market order.
    EXP void market_buy(LimitOrderBook* book, UID order_id, Size size) {
        book->market_buy(order_id, size, 0);
    }

    /// Execute a market order.
    EXP void market(LimitOrderBook* book, Side side, UID order_id, Size size) {
        book->market(side, order_id, size, 0);
    }

    /// Return the best sell price.
    EXP Price best_sell(LimitOrderBook* book) {
        return book->best_sell();
    }

    /// Return the best buy price.
    EXP Price best_buy(LimitOrderBook* book) {
        return book->best_buy();
    }

    /// Return the best price for the given side.
    EXP Price best(LimitOrderBook* book, Side side) {
        return book->best(side);
    }

    /// Return the total volume for the sell side of the book.
    EXP Volume volume_sell_price(LimitOrderBook* book, Price price) {
        return book->volume_sell(price);
    }

    /// Return the total volume for the sell side of the book.
    EXP Volume volume_sell(LimitOrderBook* book) {
        return book->volume_sell();
    }

    /// Return the total volume for the buy side of the book.
    EXP Volume volume_buy_price(LimitOrderBook* book, Price price) {
        return book->volume_buy(price);
    }

    /// Return the total volume for the buy side of the book.
    EXP Volume volume_buy(LimitOrderBook* book) {
        return book->volume_buy();
    }

    /// Return the volume at the given limit price.
    EXP Volume volume_price(LimitOrderBook* book, Price price) {
        return book->volume(price);
    }

    /// Return the total volume for the book.
    EXP Volume volume(LimitOrderBook* book) {
        return book->volume();
    }

    /// Return the size at the given limit price.
    EXP Size size_at(LimitOrderBook* book, Price price) {
        return book->size_at(price);
    }

    /// Return the total size of the sell side of the book (number of orders).
    EXP Size size_sell(LimitOrderBook* book) {
        return book->size_sell();
    }

    /// Return the total size of the buy side of the book (number of orders).
    EXP Size size_buy(LimitOrderBook* book) {
        return book->size_buy();
    }

    /// Return the total size of the book (number of orders).
    EXP Size size(LimitOrderBook* book) {
        return book->size();
    }
}

// un-define the macro
#undef EXP
