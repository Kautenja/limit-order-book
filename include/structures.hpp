// Types and structures for the LimitOrderBook: order, limit
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
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//

#ifndef LOB_STRUCTURES_HPP_
#define LOB_STRUCTURES_HPP_

#include "doubly_linked_list.hpp"
#include "binary_search_tree.hpp"
#include <cstdint>

/// Logic for maintaining a continuous double auction via a limit-order book.
namespace LOB {

/// the possible sides for the LimitTree
enum class Side : bool { Sell = false, Buy = true };

/// @brief Return the opposite side using the invert operator.
///
/// @param side the side to invert
/// @returns the opposite of the given side
///
inline Side operator!(Side side) {
    return static_cast<Side>(!static_cast<bool>(side));
}

/// a type for order IDs
typedef uint64_t UID;
/// a type for order quantities
typedef uint32_t Quantity;
/// a type for order prices
typedef uint64_t Price;

// forward declare the `Limit` structure so `Order` can reference it
struct Limit;

/// A single order in the LimitOrderBook.
struct Order : DLL::Node {
    /// the day-unique ID for this particular order
    const UID uid = 0;
    /// a boolean determining whether the order id a buy (true) or sell (false)
    const Side side = Side::Sell;
    /// the number of shares in the order
    Quantity quantity = 0;
    /// the limit price for the order (market price if market order)
    const Price price = 0;
    /// the limit this order falls under
    Limit* limit = nullptr;

    /// Initialize a new order data.
    Order() : DLL::Node() { }

    /// @brief Initialize a new order data.
    ///
    /// @param uid_ the unique identifier for the order
    /// @param side_ the side of the order: buy=true, sell=false
    /// @param quantity_ the number of shares to buy or sell
    /// @param price_ the limit/market price for the order
    ///
    Order(UID uid_, Side side_, Quantity quantity_, Price price_) :
        DLL::Node(),
        uid(uid_),
        side(side_),
        quantity(quantity_),
        price(price_) { }
};

/// a type for limit price order counts
typedef uint32_t Count;
/// a type for limit total volume
typedef uint64_t Volume;

/// A price limit containing a FIFO queue of Order objects.
struct Limit : BST::Node<Price> {
    /// the number of orders at this limit price
    Count count = 0;
    /// padding for byte alignment
    const uint32_t _padding = 0;
    /// the total amount of volume at this limit price (sum of order shares)
    Volume volume = 0;
    /// the first order in the queue (next to execute)
    Order* order_head = nullptr;
    /// the last order in the queue (last to execute)
    Order* order_tail = nullptr;

    /// @brief Initialize a new limit data.
    Limit() : BST::Node<Price>() { }

    /// @brief Initialize a new limit data.
    ///
    /// @param order the initial order to create the limit with
    ///
    explicit Limit(Order* order) :
        BST::Node<Price>(order->price),
        count(1),
        volume(order->quantity),
        order_head(order),
        order_tail(order) { }
};

}  // namespace LOB

#endif  // LOB_STRUCTURES_HPP_
