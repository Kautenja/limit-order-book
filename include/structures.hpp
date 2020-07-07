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

namespace LOB {

/// the possible sides for the LimitTree
enum class Side : bool { Sell, Buy };

/// Return the opposite side using the invert operator
///
/// @param side the side to invert
/// @returns the opposite of the given side
///
inline Side operator!(Side side) {
    return static_cast<Side>(!static_cast<bool>(side));
}

/// a type for order UIDs
typedef uint64_t UID;
/// a type for order event timestamps in nanoseconds
typedef uint64_t Timestamp;
/// a type for order sizes (quantities)
typedef uint32_t Size;
/// a type for order prices
typedef uint64_t Price;
/// a type for limit total volume
typedef uint32_t Volume;

// forward declare the `Limit` structure so `Order` can reference it
struct Limit;

/// A representation of an order in the Limit Order Book.
struct Order : DLL::Node {
    /// the day-unique ID for this particular order
    UID uid;
    /// a boolean determining whether the order id a buy (true) or sell (false)
    Side side;
    /// the size of the order, i.e., the number of shares
    Size size;
    /// the limit price for the order (market price if market order)
    Price price;
    /// the time stamp for when the order arrived
    Timestamp arrival;
    /// the time stamp for when the order executed
    Timestamp execution;
    /// the limit this order falls under
    Limit* limit;

    /// Initialize a new order data.
    Order() :
        DLL::Node(),
        uid(0),
        side(),
        size(0),
        price(0),
        arrival(0),
        execution(0),
        limit(nullptr) { }

    /// Initialize a new order data.
    ///
    /// @param uid_ the unique identifier for the order
    /// @param side_ the side of the order: buy=true, sell=false
    /// @param size_ the number of shares to buy or sell
    /// @param price_ the limit/market price for the order
    /// @param arrival_ the time of arrival for the order
    /// @param execution_ the time of execution for the order
    ///
    Order(
        UID uid_,
        Side side_,
        Size size_,
        Price price_,
        Timestamp arrival_ = 0,
        Timestamp execution_ = 0
    ) :
        DLL::Node(),
        uid(uid_),
        side(side_),
        size(size_),
        price(price_),
        arrival(arrival_),
        execution(execution_),
        limit(nullptr) { }

 private:
    /// disable the copy constructor
    Order(const Order& other) { }
};

/// A representation of a price limit with a FIFO queue of orders.
struct Limit : BST::Node<Price> {
    /// the number of orders at this limit
    Size size;
    /// the total amount of volume at this limit price
    Volume volume;
    /// the first order in the queue (next to execute)
    Order* order_head;
    /// the last order in the queue (last to execute)
    Order* order_tail;

    /// Initialize a new limit data.
    Limit() :
        BST::Node<Price>(),
        size(0),
        volume(0),
        order_head(nullptr),
        order_tail(nullptr) { }

    /// Initialize a new limit data.
    ///
    /// @param order the initial order to create the limit with
    ///
    Limit(Order* order) :
        BST::Node<Price>(order->price),
        size(1),
        volume(order->size),
        order_head(order),
        order_tail(order) { }

 private:
    /// disable the copy constructor
    Limit(const Limit& other) { }
};

}  // namespace LOB

#endif  // LOB_STRUCTURES_HPP_
