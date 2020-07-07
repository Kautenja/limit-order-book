// A single side (buy/sell) of the LimitOrderBook.
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

#ifndef LOB_LIMIT_TREE_HPP_
#define LOB_LIMIT_TREE_HPP_

#include "structures.hpp"
#include "tsl/robin_map.h"

/// Logic for maintaining a continuous double auction via a limit-order book.
namespace LOB {

/// a map of prices to limit pointers
typedef tsl::robin_map<Price, Limit*> PriceLimitMap;

// ---------------------------------------------------------------------------
// MARK: set_best
// ---------------------------------------------------------------------------

/// @brief Set the best to the given limit if the given limit is better.
///
/// @tparam side the side of the book to perform the calculation from
/// @param best the current best limit
/// @param limit the limit node to compare to
///
template<Side side>
void set_best(Limit** best, Limit* limit);

/// @brief Set the best buy to the given limit if the given limit is better.
///
/// @param highest_buy the current best buy limit
/// @param limit the buy limit node to compare to
///
template<>
inline void set_best<Side::Buy>(Limit** highest_buy, Limit* limit) {
    if (*highest_buy == nullptr) *highest_buy = limit;
    else if (limit->key > (*highest_buy)->key) *highest_buy = limit;
}

/// @brief Set the best sell to the given limit if the given limit is better.
///
/// @param lowest_sell the current best sell limit
/// @param limit the sell limit node to compare to
///
template<>
inline void set_best<Side::Sell>(Limit** lowest_sell, Limit* limit) {
    if (*lowest_sell == nullptr) *lowest_sell = limit;
    else if (limit->key < (*lowest_sell)->key) *lowest_sell = limit;
}

// ---------------------------------------------------------------------------
// MARK: find_best
// ---------------------------------------------------------------------------

/// @brief Find the next best when removing the best from the tree.
///
/// @tparam side the side of the book to perform the calculation from
/// @param best the best limit node
///
template<Side side>
void find_best(Limit** best);

/// @brief Find the next buy when removing the best buy from the tree.
///
/// @param highest_buy the best buy limit node
///
template<>
inline void find_best<Side::Buy>(Limit** highest_buy) {
    if ((*highest_buy)->parent == nullptr)  // removing root node
        // set left because if right existed it would be a better limit already
        *highest_buy = static_cast<Limit*>((*highest_buy)->left);
    else  // removing non-root node
        *highest_buy = static_cast<Limit*>((*highest_buy)->parent);
    if (*highest_buy != nullptr)  // highest buy exists
        *highest_buy = static_cast<Limit*>(BST::max(*highest_buy));
}

/// @brief Find the next sell when removing the best sell from the tree.
///
/// @param lowest_sell the best sell limit node
///
template<>
inline void find_best<Side::Sell>(Limit** lowest_sell) {
    if ((*lowest_sell)->parent == nullptr)  // removing root node
        // set right because if left existed it would be a better limit already
        *lowest_sell = static_cast<Limit*>((*lowest_sell)->right);
    else  // removing non-root node
        *lowest_sell = static_cast<Limit*>((*lowest_sell)->parent);
    if (*lowest_sell != nullptr)  // lowest sell exists
        *lowest_sell = static_cast<Limit*>(BST::min(*lowest_sell));
}

// ---------------------------------------------------------------------------
// MARK: can_match
// ---------------------------------------------------------------------------

/// @brief Return true if the market price can match with the limit price.
///
/// @tparam side the side of the book to perform the calculation from
/// @param limit the price of the limit order (i.e., the best price)
/// @param market the price of the opposing side market order to compare
/// @returns true if the market price can match with the limit price
///
template<Side side>
bool can_match(Price limit, Price market);

/// @brief Return true if the sell market price can match with the buy limit price.
///
/// @param limit the price of the buy limit order (i.e., the best price)
/// @param market the price of the sell side market order to compare
/// @returns true if the sell market price can match with the buy limit price
///
template<>
inline bool can_match<Side::Buy>(Price limit, Price market) {
    // short circuit on price > 0: 0 is sentinel for no limit
    // the price should be less than or equal to the best on the buy side.
    // i.e., the submitted order is a sell limit with a lower price
    return market == 0 || market <= limit;
}

/// @brief Return true if the buy market price can match with the sell limit price.
///
/// @param limit the price of the sell limit order (i.e., the best price)
/// @param market the price of the buy side market order to compare
/// @returns true if the buy market price can match with the sell limit price
///
template<>
inline bool can_match<Side::Sell>(Price limit, Price market) {
    // short circuit on price > 0: 0 is sentinel for no limit
    // the price should be greater than or equal to the best on the sell side.
    // i.e., the submitted order is a buy limit with a higher price
    return market == 0 || market >= limit;
}

/// A single side (buy/sell) of the LimitOrderBook.
template<Side side>
struct LimitTree {
    /// the sorted tree of orders in the book
    Limit* root = nullptr;
    /// a mapping of limit prices to limits
    PriceLimitMap limits;
    /// the best order
    Limit* best = nullptr;
    /// the last best price
    Price last_best_price = 0;
    /// the total number of active orders for this tree
    Count count = 0;
    /// the total volume of orders for this tree
    Volume volume = 0;

    /// @brief Clear all the limits in the tree
    void clear() {
        // delete all the limits
        for(auto item = limits.begin(); item != limits.end(); item++)
            delete item->second;
        // clear the pairs from the map
        limits.clear();
        // set remaining tracers to 0
        root = nullptr;
        best = nullptr;
        count = 0;
        volume = 0;
    }

    /// @brief Place a limit order on the limit tree.
    ///
    /// @param order an order that matches the side of this tree
    ///
    void limit(Order* order) {
        if (limits.count(order->price) == 0) {  // first order at limit price
            // create the limit and set the order's limit to the limit
            order->limit = new Limit(order);
            // the limit price is new, add it to the limit tree
            BST::insert(
                reinterpret_cast<BST::Node<Price>**>(&root),
                static_cast<BST::Node<Price>*>(order->limit)
            );
            // set the best price
            set_best<side>(&best, order->limit);
            // put the limit into the price to limit mapping
            limits.emplace(order->limit->key, order->limit);
        } else {  // push to existing queue of orders at limit price
            // set the order's limit to the limit node for the order's price
            order->limit = limits.at(order->price);
            // increment the count of the orders at this limit node
            ++order->limit->count;
            // increment the volume of the orders at this limit node
            order->limit->volume += order->quantity;
            // add the order to the queue of orders for the limit node
            DLL::append(
                reinterpret_cast<DLL::Node**>(&order->limit->order_tail),
                static_cast<DLL::Node*>(order)
            );
        }
        // update the count and volume for the entire tree
        ++count;
        volume += order->quantity;
        // set the last best price
        if (best != nullptr) last_best_price = best->key;
    }

    // TODO: use object pool for limit reuse?
    /// @brief Remove an order from the limit tree.
    ///
    /// @param order an order that exists in the limit tree
    ///
    void cancel(Order* order) {
        // unwrap the limit for the order
        auto limit_ = order->limit;
        if (order->prev == nullptr && order->next == nullptr) {  // last Order
            // remove the limit from the tree
            BST::remove(
                reinterpret_cast<BST::Node<Price>**>(&root),
                static_cast<BST::Node<Price>*>(limit_)
            );
            // replace the best limit if necessary
            if (best == limit_)
                find_best<side>(&best);
            // remove the limit from the price map
            limits.erase(limit_->key);
            delete limit_;
        } else {  // at least 1 other Order in queue
            --limit_->count;
            limit_->volume -= order->quantity;
            // remove the order from the queue (update head and tail)
            DLL::remove(
                reinterpret_cast<DLL::Node**>(&limit_->order_head),
                reinterpret_cast<DLL::Node**>(&limit_->order_tail),
                static_cast<DLL::Node*>(order)
            );
        }
        // update the count and volume for the entire tree
        --count;
        volume -= order->quantity;
        // set the last best price
        if (best != nullptr) last_best_price = best->key;
    }

    /// @brief Perform a market order of given quantity on the given limit tree.
    ///
    /// @tparam Callback a callable function that excepts an order ID
    /// @param order the order to find a market order for
    /// @param did_fill the callback handler for when an order is filled
    ///
    template<typename Callback>
    void market(Order* order, Callback did_fill) {
        // find orders until there are none
        while (best != nullptr && can_match<side>(best->key, order->price)) {
            // get the next match as the front of the best price
            auto match = best->order_head;
            if (match->quantity >= order->quantity) {  // current match can fill
                if (match->quantity == order->quantity) {  // limit order filled
                    // remove the current match from the book
                    cancel(match);
                    did_fill(match->uid);
                } else {  // limit order partially filled
                    // remove the market order quantity from the limit quantity
                    match->quantity -= order->quantity;
                    // update the match's limit volume
                    match->limit->volume -= order->quantity;
                    // update the volume for the entire tree
                    volume -= order->quantity;
                }
                // clear the remaining quantity for the order
                order->quantity = 0;
                return;
            }  // else: current match can NOT fill
            // decrement the remaining quantity of the market order
            order->quantity -= match->quantity;
            // remove the current match from the book
            cancel(match);
            did_fill(match->uid);
        }

    }

    /// @brief Return the volume of orders at the given limit price.
    ///
    /// @param price the limit price to return the volume for
    /// @returns the volume of orders at the given limit price
    ///
    inline Volume volume_at(Price price) const {
        if (limits.count(price)) return limits.at(price)->volume;
        return 0;
    }

    /// @brief Return the number of orders at the given limit price.
    ///
    /// @param price the limit price to return the number of orders for
    /// @returns the number of orders at the given limit price
    ///
    inline Count count_at(Price price) const {
        if (limits.count(price)) return limits.at(price)->count;
        return 0;
    }
};

}  // namespace LOB

#endif  // LOB_LIMIT_TREE_HPP_
