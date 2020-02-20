// The Limit Order Book (LOB).
// Copyright 2019 Christian Kauten
//

#ifndef LIMIT_ORDER_BOOK_LIMIT_ORDER_BOOK
#define LIMIT_ORDER_BOOK_LIMIT_ORDER_BOOK

#include <unordered_map>
#include "limit_tree.hpp"

namespace LOB {

/// a map of order IDs to orders
typedef std::unordered_map<UID, Order> UIDOrderMap;

/// The Limit Order Book (LOB).
class LimitOrderBook {
 private:
    /// the tree of sell order in the book.
    LimitTree<Side::Sell> sells;
    /// the tree of buy orders in the book.
    LimitTree<Side::Buy> buys;
    /// a mapping of order IDs to orders (for cancellation).
    UIDOrderMap orders;

 public:
    /// Initialize a new limit order book object.
    LimitOrderBook() : sells(), buys(), orders() { }

    /// Clear all the orders in the book.
    void clear() {
        sells.clear();
        buys.clear();
        orders.clear();
    }

    /// Add a new sell limit order to the book.
    ///
    /// @param order_id the ID for the order
    /// @param size the number of shares to sell
    /// @param price the limit price for the order
    /// @param arrival the time the order arrived at
    /// @return the order ID for the order added to the book
    ///
    void limit_sell(UID order_id, Size size, Price price, Timestamp arrival) {
        // put the order into the map
        // orders.insert({order_id, {order_id, Side::Sell, size, price, arrival}});
        orders.emplace(std::piecewise_construct,
            std::forward_as_tuple(order_id),
            std::forward_as_tuple(order_id, Side::Sell, size, price, arrival)
        );
        if (buys.best != nullptr && price <= buys.best->key) {  // crosses
            // place a market order with the limit price
            buys.market(&orders.at(order_id), [&](UID uid) { orders.erase(uid); });
            if (orders.at(order_id).size == 0) {  // order filled
                orders.erase(order_id);
                return;
            }
        }
        sells.limit(&orders.at(order_id));
    }

    /// Add a new buy limit order to the book.
    ///
    /// @param order_id the ID for the order
    /// @param size the number of shares to buy
    /// @param price the limit price for the order
    /// @param arrival the time the order arrived at
    /// @return the order ID for the order added to the book
    ///
    void limit_buy(UID order_id, Size size, Price price, Timestamp arrival) {
        // put the order into the map
        // orders.insert({order_id, {order_id, Side::Buy, size, price, arrival}});
        orders.emplace(std::piecewise_construct,
            std::forward_as_tuple(order_id),
            std::forward_as_tuple(order_id, Side::Buy, size, price, arrival)
        );
        if (sells.best != nullptr && price >= sells.best->key) {  // crosses
            // place a market order with the limit price
            sells.market(&orders.at(order_id), [&](UID uid) { orders.erase(uid); });
            if (orders.at(order_id).size == 0) {  // order filled
                orders.erase(order_id);
                return;
            }
        }
        buys.limit(&orders.at(order_id));
    }

    /// Add a new order to the book.
    ///
    /// @param side whether the order is a buy (true) or sell (false)
    /// @param order_id the ID for the order
    /// @param size the number of shares to buy
    /// @param price the limit/market price for the order
    /// @param arrival the time the order arrived at
    /// @return the order ID for the order added to the book
    ///
    inline void limit(Side side, UID order_id, Size size, Price price, Timestamp arrival) {
        switch (side) {  // send the order to the appropriate side
            case Side::Sell: return limit_sell(order_id, size, price, arrival);
            case Side::Buy:  return limit_buy(order_id, size, price, arrival);
        }
    }

    /// Return true if the book has an order with given ID, false otherwise.
    ///
    /// @param order_id the order ID of the order to check for existence of
    /// @returns true if the book has an order with given ID, false otherwise
    ///
    inline bool has(UID order_id) { return orders.count(order_id); }

    /// Get the order with given ID.
    ///
    /// @param order_id the order ID of the order to get
    /// @returns a pointer to the order with given order ID
    ///
    inline const Order& get(UID order_id) { return orders.at(order_id); }

    /// Cancel an existing order in the book.
    ///
    /// @param order_id the ID of the order to cancel
    ///
    void cancel(UID order_id) {
        auto order = &orders.at(order_id);
        switch (order->side) {  // remove the order from the appropriate side
            case Side::Sell: { sells.cancel(order); break; }
            case Side::Buy:  { buys.cancel(order); break; }
        }
        orders.erase(order_id);
    }

    /// Execute a sell market order.
    ///
    /// @param order_id the ID for the order
    /// @param size the size of the market order
    /// @arrival the arrival of the market order
    ///
    void market_sell(UID order_id, Size size, Timestamp arrival) {
        auto order = Order(order_id, Side::Sell, size, 0, arrival);
        order.execution = arrival;
        buys.market(&order, [&](UID uid) { orders.erase(uid); });
    }

    /// Execute a buy market order.
    ///
    /// @param order_id the ID for the order
    /// @param size the size of the market order
    /// @arrival the arrival of the market order
    ///
    void market_buy(UID order_id, Size size, Timestamp arrival) {
        auto order = Order(order_id, Side::Buy, size, 0, arrival);
        order.execution = arrival;
        sells.market(&order, [&](UID uid) { orders.erase(uid); });
    }

    /// Execute a market order.
    ///
    /// @param side whether the order is a sell or buy order
    /// @param order_id the ID for the order
    /// @param size the size of the market order
    /// @arrival the arrival of the market order
    ///
    inline void market(Side side, UID order_id, Size size, Timestamp arrival) {
        switch (side) {  // send the market order to the appropriate side
            case Side::Sell: { market_sell(order_id, size, arrival); break; }
            case Side::Buy:  { market_buy(order_id, size, arrival); break; }
        }
    }

    /// Return the best sell price.
    ///
    /// @return the best bid price in the book
    ///
    inline Price best_sell() {
        if (sells.best == nullptr)
            return 0;
        return sells.best->key;
    }

    /// Return the best buy price.
    ///
    /// @return the best bid price in the book
    ///
    inline Price best_buy() {
        if (buys.best == nullptr)
            return 0;
        return buys.best->key;
    }

    /// Return the best price for the given side.
    ///
    /// @param side the side to get the best price from
    /// @returns the best price on the given side of the book
    ///
    Price best(Side side) {
        switch (side) {
            case Side::Sell: { return best_sell(); }
            case Side::Buy:  { return best_buy();  }
        }
    }

    /// Return the total volume for the sell side of the book.
    ///
    /// @param price the limit price to get the volume for
    /// @return the volume for the given limit price
    ///
    inline Volume volume_sell(Price price) { return sells.volume_at(price); }

    /// Return the total volume for the sell side of the book.
    inline Volume volume_sell() { return sells.volume; }

    /// Return the total volume for the buy side of the book.
    ///
    /// @param price the limit price to get the volume for
    /// @return the volume for the given limit price
    ///
    inline Volume volume_buy(Price price) { return buys.volume_at(price); }

    /// Return the total volume for the buy side of the book.
    inline Volume volume_buy() { return buys.volume; }

    /// Return the volume at the given limit price.
    ///
    /// @param price the limit price to get the volume for
    /// @return the volume for the given limit price
    ///
    inline Volume volume(Price price) {
        return buys.volume_at(price) + sells.volume_at(price);
    }

    /// Return the total volume for the book.
    inline Volume volume() { return sells.volume + buys.volume; }

    /// Return the size at the given limit price.
    ///
    /// @param price the limit price to get the volume for
    /// @return the volume for the given limit price
    ///
    inline Size size_at(Price price) {
        return buys.size_at(price) + sells.size_at(price);
    }

    /// Return the total size of the sell side of the book (number of orders).
    inline Size size_sell() { return sells.size; }

    /// Return the total size of the buy side of the book (number of orders).
    inline Size size_buy() { return buys.size; }

    /// Return the total size of the book (number of orders).
    inline Size size() { return sells.size + buys.size; }
};

}  // namespace LOB

#endif  // LIMIT_ORDER_BOOK_LIMIT_ORDER_BOOK
