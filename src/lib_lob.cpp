// The definitions of the C API declared in lib_lob.h.
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

#include "lib_lob.h"
#include "limit_order_book.hpp"
#include <type_traits>

// the C types must match the widths of the C++ aliases they stand for
static_assert(std::is_same<LOB_UID, LOB::UID>::value, "LOB_UID != UID");
static_assert(std::is_same<LOB_Quantity, LOB::Quantity>::value, "LOB_Quantity != Quantity");
static_assert(std::is_same<LOB_Price, LOB::Price>::value, "LOB_Price != Price");
static_assert(std::is_same<LOB_Volume, LOB::Volume>::value, "LOB_Volume != Volume");
static_assert(std::is_same<LOB_Count, LOB::Count>::value, "LOB_Count != Count");
static_assert(
    std::is_same<LOB_Side, std::underlying_type<LOB::Side>::type>::value,
    "LOB_Side != the underlying type of Side"
);

/// @brief The object behind the opaque C handle to a limit order book.
struct LOB_LimitOrderBook {
    /// the C++ limit order book
    LOB::LimitOrderBook book;
};

/// @brief Convert a C side flag to the C++ side.
///
/// @param side whether the order is a buy (true) or sell (false)
/// @returns the matching C++ side
///
static inline LOB::Side to_side(LOB_Side side) {
    return static_cast<LOB::Side>(side);
}

// ---------------------------------------------------------------------------
// MARK: Lifetime
// ---------------------------------------------------------------------------

LOB_LimitOrderBook* new_(void) { return new LOB_LimitOrderBook(); }

void delete_(LOB_LimitOrderBook* book) { delete book; }

void clear(LOB_LimitOrderBook* book) { book->book.clear(); }

// ---------------------------------------------------------------------------
// MARK: Limit Orders
// ---------------------------------------------------------------------------

void limit_sell(
    LOB_LimitOrderBook* book,
    LOB_UID uid,
    LOB_Quantity quantity,
    LOB_Price price
) { book->book.limit_sell(uid, quantity, price); }

void limit_buy(
    LOB_LimitOrderBook* book,
    LOB_UID uid,
    LOB_Quantity quantity,
    LOB_Price price
) { book->book.limit_buy(uid, quantity, price); }

void limit(
    LOB_LimitOrderBook* book,
    LOB_Side side,
    LOB_UID uid,
    LOB_Quantity quantity,
    LOB_Price price
) { book->book.limit(to_side(side), uid, quantity, price); }

bool has(LOB_LimitOrderBook* book, LOB_UID uid) { return book->book.has(uid); }

const LOB_Order* get(LOB_LimitOrderBook* book, LOB_UID uid) {
    // check first so an unknown ID cannot throw across the C boundary
    if (!book->book.has(uid)) return nullptr;
    return reinterpret_cast<const LOB_Order*>(&book->book.get(uid));
}

void cancel(LOB_LimitOrderBook* book, LOB_UID uid) {
    // check first so an unknown ID cannot throw across the C boundary
    if (book->book.has(uid)) book->book.cancel(uid);
}

// ---------------------------------------------------------------------------
// MARK: Market Orders
// ---------------------------------------------------------------------------

void market_sell(
    LOB_LimitOrderBook* book,
    LOB_UID uid,
    LOB_Quantity quantity
) { book->book.market_sell(uid, quantity); }

void market_buy(
    LOB_LimitOrderBook* book,
    LOB_UID uid,
    LOB_Quantity quantity
) { book->book.market_buy(uid, quantity); }

void market(
    LOB_LimitOrderBook* book,
    LOB_Side side,
    LOB_UID uid,
    LOB_Quantity quantity
) { book->book.market(to_side(side), uid, quantity); }

// ---------------------------------------------------------------------------
// MARK: Best Prices
// ---------------------------------------------------------------------------

LOB_Price best_sell(LOB_LimitOrderBook* book) { return book->book.best_sell(); }

LOB_Price best_buy(LOB_LimitOrderBook* book) { return book->book.best_buy(); }

LOB_Price best(LOB_LimitOrderBook* book, LOB_Side side) {
    return book->book.best(to_side(side));
}

// ---------------------------------------------------------------------------
// MARK: Volumes
// ---------------------------------------------------------------------------

LOB_Volume volume_sell_price(LOB_LimitOrderBook* book, LOB_Price price) {
    return book->book.volume_sell(price);
}

LOB_Volume volume_sell(LOB_LimitOrderBook* book) {
    return book->book.volume_sell();
}

LOB_Volume volume_buy_price(LOB_LimitOrderBook* book, LOB_Price price) {
    return book->book.volume_buy(price);
}

LOB_Volume volume_buy(LOB_LimitOrderBook* book) {
    return book->book.volume_buy();
}

LOB_Volume volume_price(LOB_LimitOrderBook* book, LOB_Price price) {
    return book->book.volume(price);
}

LOB_Volume volume(LOB_LimitOrderBook* book) { return book->book.volume(); }

// ---------------------------------------------------------------------------
// MARK: Counts
// ---------------------------------------------------------------------------

LOB_Count count_at(LOB_LimitOrderBook* book, LOB_Price price) {
    return book->book.count_at(price);
}

LOB_Count count_sell(LOB_LimitOrderBook* book) {
    return book->book.count_sell();
}

LOB_Count count_buy(LOB_LimitOrderBook* book) { return book->book.count_buy(); }

LOB_Count count(LOB_LimitOrderBook* book) { return book->book.count(); }
