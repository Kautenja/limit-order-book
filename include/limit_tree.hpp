// A (buy/sell) side of a limit order book.
// Copyright 2019 Christian Kauten
//

#ifndef LIMIT_ORDER_BOOK_LIMIT_TREE
#define LIMIT_ORDER_BOOK_LIMIT_TREE

#include "tsl/robin_map.h"
#include "structures.hpp"

namespace LOB {

/// a map of prices to limit pointers
typedef tsl::robin_map<Price, Limit*> PriceLimitMap;

/// Set the best to the given limit if the given limit is better.
///
/// @param best the current best limit
/// @param limit the limit node to compare to
///
template<Side>
void set_best(Limit** best, Limit* limit);
// Compare best buy to limit and set if it's better.
template<>
inline void set_best<Side::Buy>(Limit** highest_buy, Limit* limit) {
    if (*highest_buy == nullptr) *highest_buy = limit;
    else if (limit->key > (*highest_buy)->key) *highest_buy = limit;
}
// Compare best sell to limit and set if it's better.
template<>
inline void set_best<Side::Sell>(Limit** lowest_sell, Limit* limit) {
    if (*lowest_sell == nullptr) *lowest_sell = limit;
    else if (limit->key < (*lowest_sell)->key) *lowest_sell = limit;
}

/// Find the next best when removing the best from the tree.
///
/// @param best the best limit node
///
template<Side>
void find_best(Limit** best);
// Replace the highest buy
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
// Replace the lowest sell
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

/// Return true if
///
/// @param best the best limit price of the given side
/// @param price the price of the opposing side limit order to compare to
///
template<Side>
bool is_in_limit(Price best, Price price);
// Replace the highest buy
template<>
inline bool is_in_limit<Side::Buy>(Price best, Price price) {
    // short circuit on price > 0: 0 is sentinel for no limit
    // the price should be less than or equal to the best on the buy side.
    // i.e., the submitted order is a sell limit with a lower price
    return price == 0 || price <= best;
}
// Replace the lowest sell
template<>
inline bool is_in_limit<Side::Sell>(Price best, Price price) {
    // short circuit on price > 0: 0 is sentinel for no limit
    // the price should be greater than or equal to the best on the sell side.
    // i.e., the submitted order is a buy limit with a higher price
    return price == 0 || price >= best;
}

/// The Limit Order Book (LOB).
template<Side side>
struct LimitTree {
    /// the sorted tree of orders in the book
    Limit* root;
    /// a mapping of limit prices to limits
    PriceLimitMap limits;
    /// the best order
    Limit* best;
    /// the total number of active orders for this tree
    Size size;
    /// the total volume of orders for this tree
    Volume volume;

    /// Initialize a new limit tree.
    LimitTree() :
        root(nullptr),
        limits(),
        best(nullptr),
        size(0),
        volume(0) { }

    /// Clear all the limits in the tree
    void clear() {
        // delete all the limits
        for(auto item = limits.begin(); item != limits.end(); item++)
            delete item->second;
        // clear the pairs from the map
        limits.clear();
        // set remaining tracers to 0
        root = nullptr;
        best = nullptr;
        size = 0;
        volume = 0;
    }

    /// Place a limit order on the limit tree.
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
            // increment the size of the orders at this limit node
            ++order->limit->size;
            // increment the volume of the orders at this limit node
            order->limit->volume += order->size;
            // add the order to the queue of orders for the limit node
            DLL::append(
                reinterpret_cast<DLL::Node**>(&order->limit->order_tail),
                static_cast<DLL::Node*>(order)
            );
        }
        // update the size and volume for the entire tree
        ++size;
        volume += order->size;
    }

    // TODO: use object pool for limit reuse?
    /// Remove an order from the limit tree.
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
            --limit_->size;
            limit_->volume -= order->size;
            // remove the order from the queue (update head and tail)
            DLL::remove(
                reinterpret_cast<DLL::Node**>(&limit_->order_head),
                reinterpret_cast<DLL::Node**>(&limit_->order_tail),
                static_cast<DLL::Node*>(order)
            );
        }
        // update the size and volume for the entire tree
        --size;
        volume -= order->size;
    }

    /// Perform a market order of given size on the given limit tree.
    ///
    /// @param order the order to find a market order for
    /// @param did_fill the callback handler for when an order is filled
    ///
    template<typename Callback>
    void market(Order* order, Callback did_fill) {
        // find orders until there are none
        while (best != nullptr && is_in_limit<side>(best->key, order->price)) {
            // get the next match as the front of the best price
            auto match = best->order_head;
            // TODO: Update timestamping?
            // set the execution time for the current match
            match->execution = order->arrival;
            if (match->size >= order->size) {  // current match can fill
                if (match->size == order->size) {  // limit order filled
                    // remove the current match from the book
                    cancel(match);
                    did_fill(match->uid);
                } else {  // limit order partially filled
                    // remove the market order quantity from the limit quantity
                    match->size -= order->size;
                    // update the match's limit volume
                    match->limit->volume -= order->size;
                    // update the volume for the entire tree
                    volume -= order->size;
                }
                // clear the size of the market order
                order->size = 0;
                return;
            }  // else: current match can NOT fill
            // decrement the remaining size of the market order
            order->size -= match->size;
            // remove the current match from the book
            cancel(match);
            did_fill(match->uid);
        }
    }

    /// Return the volume of orders at the given limit price.
    ///
    /// @param price the limit price to return the volume for
    /// @returns the volume of orders at the given limit price
    ///
    inline Volume volume_at(Price price) {
        if (limits.count(price))
            return limits.at(price)->volume;
        return 0;
    }

    /// Return the number of orders at the given limit price.
    ///
    /// @param price the limit price to return the number of orders for
    /// @returns the number of orders at the given limit price
    ///
    inline Size size_at(Price price) {
        if (limits.count(price))
            return limits.at(price)->size;
        return 0;
    }
};

}  // namespace LOB

#endif  // LIMIT_ORDER_BOOK_LIMIT_TREE
