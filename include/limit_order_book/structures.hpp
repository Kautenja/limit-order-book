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

// ---------------------------------------------------------------------------
// MARK: Types
// ---------------------------------------------------------------------------

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
/// the number of shares in a user account
typedef int64_t Shares;
/// the amount of capital in a user account
typedef int64_t Capital;

// ---------------------------------------------------------------------------
// MARK: Order
// ---------------------------------------------------------------------------

// forward declare the `Account` structure so `Order` can reference it
struct Account;
// forward declare the `Limit` structure so `Order` can reference it
struct Limit;

/// A representation of an order in the Limit Order Book.
struct Order : DLL::Node {
    /// the account this order belongs to
    Account* account;
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
        account(nullptr),
        uid(0),
        side(),
        size(0),
        price(0),
        arrival(0),
        execution(0),
        limit(nullptr) { }

    /// Initialize a new order data.
    ///
    /// @param account_ the account associated with this order
    /// @param uid_ the unique identifier for the order
    /// @param side_ the side of the order: buy=true, sell=false
    /// @param size_ the number of shares to buy or sell
    /// @param price_ the limit/market price for the order
    /// @param arrival_ the time of arrival for the order
    /// @param execution_ the time of execution for the order
    ///
    Order(
        Account* account_,
        UID uid_,
        Side side_,
        Size size_,
        Price price_,
        Timestamp arrival_ = 0,
        Timestamp execution_ = 0
    ) :
        DLL::Node(),
        account(account_),
        uid(uid_),
        side(side_),
        size(size_),
        price(price_),
        arrival(arrival_),
        execution(execution_),
        limit(nullptr) { }
};

// ---------------------------------------------------------------------------
// MARK: Limit
// ---------------------------------------------------------------------------

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
};

// ---------------------------------------------------------------------------
// MARK: Account
// ---------------------------------------------------------------------------

/// An account for a given instrument
struct Account {
    /// the number of open orders for the account
    Size order_count;
    /// the total volume of orders for the account
    Volume volume;
    /// the number of shares owned by the account
    Shares shares;
    /// the total capital the account has (funds)
    Capital capital;

    /// Create a default account
    Account() : order_count(0), volume(0), shares(0), capital(0) { }

    /// Create an account with given values.
    ///
    /// @param order_count_ the number of open orders for the account
    /// @param volume_ the total volume of orders for the account
    /// @param shares_ the number of shares owned by the account
    /// @param capital_ the total capital the account has (funds)
    ///
    Account(Size order_count_, Volume volume_, Shares shares_, Capital capital_) :
        order_count(order_count_),
        volume(volume_),
        shares(shares_),
        capital(capital_) { }

    /// Place a limit order with given size.
    ///
    /// @param size the size of the order to place
    ///
    inline void limit(Order* order) {
        ++order_count;
        volume += order->size;
    }

    /// Cancel a limit order with given size.
    ///
    /// @param size the size of the order to cancel
    ///
    inline void cancel(Order* order) {
        --order_count;
        volume -= order->size;
    }

    /// Fill a market order on given side with size and price.
    ///
    /// @param side the side of the trade
    /// @param size the size of the trade
    /// @param price the price the trade occurred at
    ///
    void fill(Side side, Size size, Price price) {
        switch (side) {
            case Side::Sell: {
                shares -= size;
                capital += size * price;
                break;
            }
            case Side::Buy:  {
                shares += size;
                capital -= size * price;
                break;
            }
        }
    }

    /// Fill a limit order.
    ///
    /// @param limit the limit order that is being filled
    /// @param market the market order that fills the limit order
    ///
    inline virtual void limit_fill(Order* limit, Order* market) {
        cancel(limit);
        fill(limit->side, limit->size, limit->price);
    }

    /// Partially fill a limit order.
    ///
    /// @param limit the limit order that is being partially filled
    /// @param market the market order that partially fills the limit order
    ///
    inline virtual void limit_partial(Order* limit, Order* market) {
        volume -= market->size;
        fill(limit->side, market->size, limit->price);
    }

    /// Fill a market order.
    ///
    /// @param limit the limit order that fills the market order
    /// @param market the market order being filled
    ///
    inline virtual void market_fill(Order* limit, Order* market) {
        fill(market->side, market->size, limit->price);
    }

    /// Partially fill a market order.
    ///
    /// @param limit the limit order that partially fills the market order
    /// @param market the market order being partially filled
    ///
    inline virtual void market_partial(Order* limit, Order* market) {
        fill(market->side, limit->size, limit->price);
    }
};

}  // namespace LOB

#endif  // LIMIT_ORDER_BOOK_STRUCTURES
