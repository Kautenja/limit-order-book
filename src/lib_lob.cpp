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
    void PyInit_lib_lob() { }
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
    EXP LimitOrderBook* Initialize() { return new LimitOrderBook(); }

    /// Delete the limit order book, i.e., purge it from memory
    EXP void Delete(LimitOrderBook* book) { delete book; }

    // limit_sell
    ///

    // limit_buy
    ///

    // limit
    ///

    // has
    ///

    // get
    ///

    // cancel
    ///

    // market_sell
    ///

    // market_buy
    ///

    // market
    ///

    // Price best_sell
    ///

    // Price best_buy
    ///

    // Price best
    ///

    /// Return the total volume for the sell side of the book.
    EXP Volume volume_sell_price(LimitOrderBook* book, Price price) { return book->volume_sell(price); }

    /// Return the total volume for the sell side of the book.
    EXP Volume volume_sell(LimitOrderBook* book) { return book->volume_sell(); }

    /// Return the total volume for the buy side of the book.
    EXP Volume volume_buy_price(LimitOrderBook* book, Price price) { return book->volume_buy(price); }

    /// Return the total volume for the buy side of the book.
    EXP Volume volume_buy(LimitOrderBook* book) { return book->volume_buy(); }

    /// Return the volume at the given limit price.
    EXP Volume volume_price(LimitOrderBook* book, Price price) { return book->volume(price); }

    /// Return the total volume for the book.
    EXP Volume volume(LimitOrderBook* book) { return book->volume(); }

    /// Return the size at the given limit price.
    EXP Size size_at(LimitOrderBook* book, Price price) { return book->size_at(price); }

    /// Return the total size of the sell side of the book (number of orders).
    EXP Size size_sell(LimitOrderBook* book) { return book->size_sell(); }

    /// Return the total size of the buy side of the book (number of orders).
    EXP Size size_buy(LimitOrderBook* book) { return book->size_buy(); }

    /// Return the total size of the book (number of orders).
    EXP Size size(LimitOrderBook* book) { return book->size(); }
}

// un-define the macro
#undef EXP
