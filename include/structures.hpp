// Types and structures for the limit order book: order, limit, and account
// Copyright 2019 Christian Kauten
//

#ifndef LIMIT_ORDER_BOOK_STRUCTURES
#define LIMIT_ORDER_BOOK_STRUCTURES

#include <cstdint>
#include "binary_search_tree.hpp"
#include "doubly_linked_list.hpp"

// TODO: arrival time necessary?
// TODO: execution time necessary?
// TODO: rearrange the order of structures for better memory / cache access?

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

#endif  // LIMIT_ORDER_BOOK_STRUCTURES
