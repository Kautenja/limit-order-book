// Test cases for a single side (buy/sell) of the LimitOrderBook.
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

#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "limit_tree.hpp"

using namespace LOB;

// ---------------------------------------------------------------------------
// MARK: constructor
// ---------------------------------------------------------------------------

SCENARIO("should initialize LimitTree") {
    WHEN("a buy limit tree is constructed") {
        THEN("the initial parameters are correct") {
            LimitTree<Side::Buy> tree;
            REQUIRE(tree.root == nullptr);
            REQUIRE(tree.limits.size() == 0);
            REQUIRE(tree.best == nullptr);
        }
    }
    WHEN("a sell limit tree is constructed") {
        THEN("the initial parameters are correct") {
            LimitTree<Side::Sell> tree;
            REQUIRE(tree.root == nullptr);
            REQUIRE(tree.limits.size() == 0);
            REQUIRE(tree.best == nullptr);
        }
    }
}

// ---------------------------------------------------------------------------
// MARK: limit
// ---------------------------------------------------------------------------

SCENARIO("add a single order to LimitTree") {
    Quantity quantity = 0x4545;
    Price price = 0xAABBCCDD00112233;
    GIVEN("a LimitTree and a single buy order") {
        auto tree = LimitTree<Side::Buy>();
        WHEN("the order is added") {
            Order node = {1, Side::Buy, quantity, price};
            tree.limit(&node);
            THEN("the order is recorded in the tree data structures") {
                REQUIRE(1 == tree.limits.size());
                REQUIRE(0 == tree.volume_at(price - 1));
                REQUIRE(quantity == tree.volume_at(price));
                REQUIRE(0 == tree.volume_at(price + 1));
                REQUIRE(0 == tree.count_at(price - 1));
                REQUIRE(1 == tree.count_at(price));
                REQUIRE(0 == tree.count_at(price + 1));
                REQUIRE(nullptr != tree.root);
                REQUIRE(nullptr != tree.best);
                REQUIRE(price == tree.best->key);
                REQUIRE(&node == tree.best->order_head);
                REQUIRE(&node == tree.best->order_tail);
                REQUIRE(tree.best == tree.root);
                REQUIRE(price == tree.last_best_price);
            }
        }
    }
    GIVEN("a LimitTree and a single sell order") {
        auto tree = LimitTree<Side::Sell>();
        WHEN("the order is added") {
            Order node = {1, Side::Sell, quantity, price};
            tree.limit(&node);
            THEN("the order is recorded in the tree data structures") {
                REQUIRE(1 == tree.limits.size());
                REQUIRE(0 == tree.volume_at(price - 1));
                REQUIRE(quantity == tree.volume_at(price));
                REQUIRE(0 == tree.volume_at(price + 1));
                REQUIRE(0 == tree.count_at(price - 1));
                REQUIRE(1 == tree.count_at(price));
                REQUIRE(0 == tree.count_at(price + 1));
                REQUIRE(nullptr != tree.root);
                REQUIRE(nullptr != tree.best);
                REQUIRE(price == tree.best->key);
                REQUIRE(&node == tree.best->order_head);
                REQUIRE(&node == tree.best->order_tail);
                REQUIRE(tree.best == tree.root);
                REQUIRE(price == tree.last_best_price);
            }
        }
    }
}

SCENARIO("add two orders to LimitTree (best first)") {
    Quantity quantity = 0x4545;
    Price price = 0xAABBCCDD00112233;
    auto priceHigher = price + 1;
    GIVEN("a LimitTree and 2 buy orders") {
        auto tree = LimitTree<Side::Buy>();
        WHEN("the orders are added") {
            Order node1 = {1, Side::Buy, quantity, priceHigher};
            tree.limit(&node1);
            Order node2 = {2, Side::Buy, quantity, price};
            tree.limit(&node2);
            THEN("the orders are recorded in the tree data structures") {
                REQUIRE(2 == tree.limits.size());
                REQUIRE(quantity == tree.volume_at(priceHigher));
                REQUIRE(quantity == tree.volume_at(price));
                REQUIRE(1 == tree.count_at(price));
                REQUIRE(1 == tree.count_at(priceHigher));
                REQUIRE(nullptr != tree.root);
                REQUIRE(nullptr != tree.best);
                REQUIRE(tree.best == tree.root);
                REQUIRE(priceHigher == tree.best->key);
                REQUIRE(&node1 == tree.best->order_head);
                REQUIRE(&node1 == tree.best->order_tail);
                REQUIRE(priceHigher == tree.last_best_price);
            }
        }
    }
    GIVEN("a LimitTree and 2 sell orders") {
        auto tree = LimitTree<Side::Sell>();
        WHEN("the orders are added") {
            Order node1 = {1, Side::Sell, quantity, price};
            tree.limit(&node1);
            Order node2 = {2, Side::Buy, quantity, priceHigher};
            tree.limit(&node2);
            THEN("the orders are recorded in the tree data structures") {
                REQUIRE(2 == tree.limits.size());
                REQUIRE(quantity == tree.volume_at(priceHigher));
                REQUIRE(quantity == tree.volume_at(price));
                REQUIRE(1 == tree.count_at(price));
                REQUIRE(1 == tree.count_at(priceHigher));
                REQUIRE(nullptr != tree.root);
                REQUIRE(nullptr != tree.best);
                REQUIRE(tree.best == tree.root);
                REQUIRE(price == tree.best->key);
                REQUIRE(&node1 == tree.best->order_head);
                REQUIRE(&node1 == tree.best->order_tail);
                REQUIRE(price == tree.last_best_price);
            }
        }
    }
}

SCENARIO("add two orders to LimitTree (best last)") {
    Quantity quantity = 0x4545;
    Price price = 0xAABBCCDD00112233;
    auto priceHigher = price + 1;
    GIVEN("a LimitTree and 2 buy orders") {
        auto tree = LimitTree<Side::Buy>();
        WHEN("the orders are added") {
            Order node1 = {1, Side::Buy, quantity, price};
            tree.limit(&node1);
            Order node2 = {2, Side::Buy, quantity, priceHigher};
            tree.limit(&node2);
            THEN("the orders are recorded in the tree data structures") {
                REQUIRE(2 == tree.limits.size());
                REQUIRE(quantity == tree.volume_at(priceHigher));
                REQUIRE(quantity == tree.volume_at(price));
                REQUIRE(1 == tree.count_at(price));
                REQUIRE(1 == tree.count_at(priceHigher));
                REQUIRE(nullptr != tree.root);
                REQUIRE(nullptr != tree.best);
                REQUIRE(tree.best != tree.root);
                REQUIRE(priceHigher == tree.best->key);
                REQUIRE(&node2 == tree.best->order_head);
                REQUIRE(&node2 == tree.best->order_tail);
                REQUIRE(priceHigher == tree.last_best_price);
            }
        }
    }
    GIVEN("a LimitTree and 2 sell orders") {
        auto tree = LimitTree<Side::Sell>();
        WHEN("the orders are added") {
            Order node1 = {1, Side::Sell, quantity, priceHigher};
            tree.limit(&node1);
            Order node2 = {2, Side::Buy, quantity, price};
            tree.limit(&node2);
            THEN("the orders are recorded in the tree data structures") {
                REQUIRE(2 == tree.limits.size());
                REQUIRE(quantity == tree.volume_at(priceHigher));
                REQUIRE(quantity == tree.volume_at(price));
                REQUIRE(1 == tree.count_at(price));
                REQUIRE(1 == tree.count_at(priceHigher));
                REQUIRE(nullptr != tree.root);
                REQUIRE(nullptr != tree.best);
                REQUIRE(tree.best != tree.root);
                REQUIRE(price == tree.best->key);
                REQUIRE(&node2 == tree.best->order_head);
                REQUIRE(&node2 == tree.best->order_tail);
                REQUIRE(price == tree.last_best_price);
            }
        }
    }
}

SCENARIO("add two orders to LimitTree (same price)") {
    Quantity quantity = 0x4545;
    Price price = 0xAABBCCDD00112233;
    GIVEN("a LimitTree and 2 orders with the same price") {
        // only need to test buy because this logic is side independent
        auto tree = LimitTree<Side::Buy>();
        WHEN("the orders are added") {
            Order node1 = {1, Side::Buy, quantity, price};
            tree.limit(&node1);
            Order node2 = {2, Side::Buy, quantity, price};
            tree.limit(&node2);
            THEN("the orders are recorded in the tree data structures") {
                REQUIRE(1 == tree.limits.size());
                REQUIRE(2 * quantity == tree.volume_at(price));
                REQUIRE(nullptr != tree.root);
                REQUIRE(nullptr != tree.best);
                REQUIRE(2 == tree.count_at(price));
                REQUIRE(tree.best == tree.root);
                REQUIRE(price == tree.best->key);
                REQUIRE(&node1 == tree.best->order_head);
                REQUIRE(&node2 == tree.best->order_tail);
                REQUIRE(price == tree.last_best_price);
            }
        }
    }
}

// ---------------------------------------------------------------------------
// MARK: cancel
// ---------------------------------------------------------------------------

SCENARIO("cancel a single order from LimitTree") {
    Quantity quantity = 0x4545;
    Price price = 0xAABBCCDD00112233;
    GIVEN("a LimitTree with a single buy order") {
        auto tree = LimitTree<Side::Buy>();
        Order node = {1, Side::Buy, quantity, price};
        tree.limit(&node);
        WHEN("the order is canceled") {
            tree.cancel(&node);
            THEN("the data structures are updated") {
                REQUIRE(0 == tree.limits.size());
                REQUIRE(0 == tree.volume_at(price));
                REQUIRE(0 == tree.count_at(price));
                REQUIRE(nullptr == tree.root);
                REQUIRE(nullptr == tree.best);
                REQUIRE(price == tree.last_best_price);
            }
        }
    }
    GIVEN("a LimitTree and with a single sell order") {
        auto tree = LimitTree<Side::Sell>();
        Order node = {1, Side::Sell, quantity, price};
        tree.limit(&node);
        WHEN("the order is canceled") {
            tree.cancel(&node);
            THEN("the data structures are updated") {
                REQUIRE(0 == tree.limits.size());
                REQUIRE(0 == tree.volume_at(price));
                REQUIRE(0 == tree.count_at(price));
                REQUIRE(nullptr == tree.root);
                REQUIRE(nullptr == tree.best);
                REQUIRE(price == tree.last_best_price);
            }
        }
    }
    GIVEN("a LimitTree and with 2 single sell orders of the same price") {
        // just need to test sell because logic is side independent
        auto tree = LimitTree<Side::Sell>();
        Order node1 = {1, Side::Sell, quantity, price};
        tree.limit(&node1);
        Order node2 = {2, Side::Sell, quantity, price};
        tree.limit(&node2);
        WHEN("the first order is canceled") {
            tree.cancel(&node1);
            THEN("the data structures are updated") {
                REQUIRE(1 == tree.limits.size());
                REQUIRE(quantity == tree.volume_at(price));
                REQUIRE(1 == tree.count_at(price));
                REQUIRE(tree.root == tree.best);
                REQUIRE(&node2 == tree.root->order_head);
                REQUIRE(&node2 == tree.root->order_tail);
                REQUIRE(price == tree.last_best_price);
            }
        }
        WHEN("the second order is canceled") {
            tree.cancel(&node2);
            THEN("the data structures are updated") {
                REQUIRE(1 == tree.limits.size());
                REQUIRE(quantity == tree.volume_at(price));
                REQUIRE(1 == tree.count_at(price));
                REQUIRE(tree.root == tree.best);
                REQUIRE(&node1 == tree.root->order_head);
                REQUIRE(&node1 == tree.root->order_tail);
                REQUIRE(price == tree.last_best_price);
            }
        }
    }
}

SCENARIO("cancel an order from LimitTree (best first)") {
    Quantity quantity = 0x4545;
    Price price = 0xAABBCCDD00112233;
    auto priceHigher = price + 1;
    GIVEN("a LimitTree with 2 buy orders") {
        auto tree = LimitTree<Side::Buy>();
        Order node1 = {1, Side::Buy, quantity, priceHigher};
        tree.limit(&node1);
        Order node2 = {2, Side::Buy, quantity, price};
        tree.limit(&node2);
        WHEN("the best order is canceled") {
            tree.cancel(&node1);
            THEN("the data structures are updated") {
                REQUIRE(1 == tree.limits.size());
                REQUIRE(0 == tree.volume_at(priceHigher));
                REQUIRE(quantity == tree.volume_at(price));
                REQUIRE(1 == tree.count_at(price));
                REQUIRE(0 == tree.count_at(priceHigher));
                REQUIRE(nullptr != tree.root);
                REQUIRE(nullptr != tree.best);
                REQUIRE(tree.best == tree.root);
                REQUIRE(price == tree.best->key);
                REQUIRE(&node2 == tree.best->order_head);
                REQUIRE(&node2 == tree.best->order_tail);
                REQUIRE(price == tree.last_best_price);
            }
        }
        WHEN("the arbitrary order is canceled") {
            tree.cancel(&node2);
            THEN("the data structures are updated") {
                REQUIRE(1 == tree.limits.size());
                REQUIRE(quantity == tree.volume_at(priceHigher));
                REQUIRE(0 == tree.volume_at(price));
                REQUIRE(0 == tree.count_at(price));
                REQUIRE(1 == tree.count_at(priceHigher));
                REQUIRE(nullptr != tree.root);
                REQUIRE(nullptr != tree.best);
                REQUIRE(tree.best == tree.root);
                REQUIRE(priceHigher == tree.best->key);
                REQUIRE(&node1 == tree.best->order_head);
                REQUIRE(&node1 == tree.best->order_tail);
                REQUIRE(priceHigher == tree.last_best_price);
            }
        }
    }
    GIVEN("a LimitTree with 2 sell orders") {
        auto tree = LimitTree<Side::Sell>();
        Order node1 = {1, Side::Sell, quantity, price};
        tree.limit(&node1);
        Order node2 = {2, Side::Buy, quantity, priceHigher};
        tree.limit(&node2);
        WHEN("the best order is canceled") {
            tree.cancel(&node1);
            THEN("the data structures are updated") {
                REQUIRE(1 == tree.limits.size());
                REQUIRE(quantity == tree.volume_at(priceHigher));
                REQUIRE(0 == tree.volume_at(price));
                REQUIRE(0 == tree.count_at(price));
                REQUIRE(1 == tree.count_at(priceHigher));
                REQUIRE(nullptr != tree.root);
                REQUIRE(nullptr != tree.best);
                REQUIRE(tree.best == tree.root);
                REQUIRE(priceHigher == tree.best->key);
                REQUIRE(&node2 == tree.best->order_head);
                REQUIRE(&node2 == tree.best->order_tail);
                REQUIRE(priceHigher == tree.last_best_price);
            }
        }
        WHEN("the arbitrary order is canceled") {
            tree.cancel(&node2);
            THEN("the data structures are updated") {
                REQUIRE(1 == tree.limits.size());
                REQUIRE(0 == tree.volume_at(priceHigher));
                REQUIRE(quantity == tree.volume_at(price));
                REQUIRE(1 == tree.count_at(price));
                REQUIRE(0 == tree.count_at(priceHigher));
                REQUIRE(nullptr != tree.root);
                REQUIRE(nullptr != tree.best);
                REQUIRE(tree.best == tree.root);
                REQUIRE(price == tree.best->key);
                REQUIRE(&node1 == tree.best->order_head);
                REQUIRE(&node1 == tree.best->order_tail);
                REQUIRE(price == tree.last_best_price);
            }
        }
    }
}

SCENARIO("cancel an order from LimitTree (best last)") {
    Quantity quantity = 0x4545;
    Price price = 0xAABBCCDD00112233;
    auto priceHigher = price + 1;
    GIVEN("a LimitTree with 2 buy orders") {
        auto tree = LimitTree<Side::Buy>();
        Order node1 = {1, Side::Buy, quantity, price};
        tree.limit(&node1);
        Order node2 = {2, Side::Buy, quantity, priceHigher};
        tree.limit(&node2);
        WHEN("the best order is canceled") {
            tree.cancel(&node2);
            THEN("the data structures are updated") {
                REQUIRE(1 == tree.limits.size());
                REQUIRE(0 == tree.volume_at(priceHigher));
                REQUIRE(quantity == tree.volume_at(price));
                REQUIRE(1 == tree.count_at(price));
                REQUIRE(0 == tree.count_at(priceHigher));
                REQUIRE(nullptr != tree.root);
                REQUIRE(nullptr != tree.best);
                REQUIRE(tree.best == tree.root);
                REQUIRE(price == tree.best->key);
                REQUIRE(&node1 == tree.best->order_head);
                REQUIRE(&node1 == tree.best->order_tail);
                REQUIRE(price == tree.last_best_price);
            }
        }
        WHEN("the arbitrary order is canceled") {
            tree.cancel(&node1);
            THEN("the data structures are updated") {
                REQUIRE(1 == tree.limits.size());
                REQUIRE(quantity == tree.volume_at(priceHigher));
                REQUIRE(0 == tree.volume_at(price));
                REQUIRE(0 == tree.count_at(price));
                REQUIRE(1 == tree.count_at(priceHigher));
                REQUIRE(nullptr != tree.root);
                REQUIRE(nullptr != tree.best);
                REQUIRE(tree.best == tree.root);
                REQUIRE(priceHigher == tree.best->key);
                REQUIRE(&node2 == tree.best->order_head);
                REQUIRE(&node2 == tree.best->order_tail);
                REQUIRE(priceHigher == tree.last_best_price);
            }
        }
    }
    GIVEN("a LimitTree with 2 sell orders") {
        auto tree = LimitTree<Side::Sell>();
        Order node1 = {1, Side::Sell, quantity, priceHigher};
        tree.limit(&node1);
        Order node2 = {2, Side::Buy, quantity, price};
        tree.limit(&node2);
        WHEN("the best order is canceled") {
            tree.cancel(&node2);
            THEN("the data structures are updated") {
                REQUIRE(1 == tree.limits.size());
                REQUIRE(quantity == tree.volume_at(priceHigher));
                REQUIRE(0 == tree.volume_at(price));
                REQUIRE(0 == tree.count_at(price));
                REQUIRE(1 == tree.count_at(priceHigher));
                REQUIRE(nullptr != tree.root);
                REQUIRE(nullptr != tree.best);
                REQUIRE(tree.best == tree.root);
                REQUIRE(priceHigher == tree.best->key);
                REQUIRE(&node1 == tree.best->order_head);
                REQUIRE(&node1 == tree.best->order_tail);
                REQUIRE(priceHigher == tree.last_best_price);
            }
        }
        WHEN("the arbitrary order is canceled") {
            tree.cancel(&node1);
            THEN("the data structures are updated") {
                REQUIRE(1 == tree.limits.size());
                REQUIRE(0 == tree.volume_at(priceHigher));
                REQUIRE(quantity == tree.volume_at(price));
                REQUIRE(1 == tree.count_at(price));
                REQUIRE(0 == tree.count_at(priceHigher));
                REQUIRE(nullptr != tree.root);
                REQUIRE(nullptr != tree.best);
                REQUIRE(tree.best == tree.root);
                REQUIRE(price == tree.best->key);
                REQUIRE(&node2 == tree.best->order_head);
                REQUIRE(&node2 == tree.best->order_tail);
                REQUIRE(price == tree.last_best_price);
            }
        }
    }
}

// ---------------------------------------------------------------------------
// MARK: market
// ---------------------------------------------------------------------------

SCENARIO("a market order is submitted with no order in the tree") {
    GIVEN("An empty limit tree") {
        LimitTree<Side::Buy> tree;
        Quantity quantity = 100;
        WHEN("a buy market order is submitted") {
            Order market = {1, Side::Sell, quantity, 0};
            tree.market(&market, [](UID) { });
            THEN("the tree is empty") {
                REQUIRE(tree.root == nullptr);
                REQUIRE(tree.limits.size() == 0);
                REQUIRE(tree.best == nullptr);
                REQUIRE(tree.count == 0);
                REQUIRE(tree.volume == 0);
                REQUIRE(tree.last_best_price == 0);
            }
        }
    }
}

SCENARIO("a market order is submitted to tree with a perfect match") {
    GIVEN("An order book with a limit order and a matched market order") {
        Quantity quantity = 100;
        Price price = 50;
        LimitTree<Side::Buy> tree;

        WHEN("a sell market order is matched to a buy limit order") {
            Order limit = {1, Side::Buy, quantity, price};
            tree.limit(&limit);
            Order market = {2, Side::Sell, quantity, 0};
            tree.market(&market, [](UID) { });
            THEN("the limit_fill and market_fill functions should fire") {
                REQUIRE(nullptr == tree.best);
                REQUIRE(0 == tree.volume_at(price));
                REQUIRE(price == tree.last_best_price);
            }
        }
    }
}

SCENARIO("a market order is submitted that is partially filled") {
    GIVEN("An order book with a limit order and a smaller market order") {
        Quantity quantity_limit = 100;
        Quantity quantity_market = 20;
        Price price = 50;
        LimitTree<Side::Buy> tree;
        Order limit = {1, Side::Buy, quantity_limit, price};
        tree.limit(&limit);

        WHEN("a buy market order is submitted") {
            Order market = {2, Side::Sell, quantity_market, 0};
            tree.market(&market, [](UID) { });
            THEN("the limit_fill and market_fill functions should fire") {
                REQUIRE(nullptr != tree.best);
                REQUIRE(price == tree.best->key);
                REQUIRE(quantity_limit - quantity_market == tree.volume_at(price));
                REQUIRE(price == tree.last_best_price);
            }
        }
    }
}

SCENARIO("a market order is submitted that spans several limit orders") {
    GIVEN("An order book with two limits and a market order that requires both") {
        Quantity quantity_limit1 = 40;
        Quantity quantity_limit2 = 20;
        Quantity quantity_market = 50;
        Price price = 100;
        LimitTree<Side::Buy> tree;
        Order limit1 = {1, Side::Buy, quantity_limit1, price};
        tree.limit(&limit1);
        Order limit2 = {2, Side::Buy, quantity_limit2, price};
        tree.limit(&limit2);

        WHEN("a buy market order is submitted") {
            Order market = {3, Side::Sell, quantity_market, 0};
            tree.market(&market, [](UID) { });
            THEN("the limit_fill and market_fill functions should fire") {
                REQUIRE(nullptr != tree.best);
                REQUIRE(100 == tree.best->key);
                REQUIRE(quantity_limit1 + quantity_limit2 - quantity_market == tree.volume_at(price));
                REQUIRE(price == tree.last_best_price);
            }
        }
    }
}

SCENARIO("a market order is submitted that spans several limit orders and clears book") {
    GIVEN("An order book with two limits and a market order that requires both") {
        Quantity quantity_limit1 = 20;
        Quantity quantity_limit2 = 20;
        Quantity quantity_market = 50;
        Price price = 100;
        LimitTree<Side::Buy> tree;
        Order limit1 = {1, Side::Buy, quantity_limit1, price};
        tree.limit(&limit1);
        Order limit2 = {2, Side::Buy, quantity_limit2, price};
        tree.limit(&limit2);

        WHEN("a buy market order is submitted") {
            Order market = {3, Side::Sell, quantity_market, 0};
            tree.market(&market, [](UID) { });
            THEN("the limit_fill and market_fill functions should fire") {
                REQUIRE(nullptr == tree.best);
                REQUIRE(0 == tree.volume_at(price));
                REQUIRE(price == tree.last_best_price);
            }
        }
    }
}

SCENARIO("a market order is submitted that spans several limit prices and clears book") {
    GIVEN("An order book with two limits and a market order that requires both") {
        Quantity quantity = 20;
        Price price1 = 101;
        Price price2 = 102;
        LimitTree<Side::Buy> tree;
        Order limit1 = {1, Side::Buy, quantity, price1};
        tree.limit(&limit1);
        Order limit2 = {2, Side::Buy, quantity, price2};
        tree.limit(&limit2);

        WHEN("a buy market order is submitted") {
            Order market = {3, Side::Sell, 2 * quantity, 0};
            tree.market(&market, [](UID) { });
            THEN("the limit_fill and market_fill functions should fire") {
                REQUIRE(nullptr == tree.best);
                REQUIRE(0 == tree.volume_at(price1));
                REQUIRE(0 == tree.volume_at(price2));
                REQUIRE(price1 == tree.last_best_price);
            }
        }
    }
}

SCENARIO("a market order is submitted with a limit price") {
    GIVEN("A book with two limits and a market order with limit price") {
        Quantity quantity_limit1 = 20;
        Quantity quantity_limit2 = 20;
        Quantity quantity_market = 40;
        Price price1 = 100;
        Price price2 = 101;
        LimitTree<Side::Buy> tree;
        Order limit1 = {1, Side::Buy, quantity_limit1, price1};
        tree.limit(&limit1);
        Order limit2 = {2, Side::Buy, quantity_limit2, price2};
        tree.limit(&limit2);

        WHEN("a buy market order is submitted") {
            Order market = {3, Side::Sell, quantity_market, price2};
            tree.market(&market, [](UID) { });
            THEN("the limit_fill and market_fill functions should fire") {
                REQUIRE(nullptr != tree.best);
                REQUIRE(100 == tree.best->key);
                REQUIRE(20 == tree.volume_at(price1));
                REQUIRE(0 == tree.volume_at(price2));
                REQUIRE(price1 == tree.last_best_price);
            }
            THEN("the order should have shares leftover") {
                REQUIRE(quantity_market - quantity_limit2 == market.quantity);
                REQUIRE(price2 == market.price);
            }
        }
    }
}

SCENARIO("a market order is submitted with a limit price that spans") {
    GIVEN("A book with two limits and a market order with limit price") {
        Quantity quantity_limit1 = 20;
        Quantity quantity_limit2 = 20;
        Quantity quantity_limit3 = 20;
        Quantity quantity_market = 60;
        Price price1 = 100;
        Price price2 = 101;
        Price price3 = 102;
        LimitTree<Side::Buy> tree;
        Order limit1 = {1, Side::Buy, quantity_limit1, price1};
        tree.limit(&limit1);
        Order limit2 = {2, Side::Buy, quantity_limit2, price2};
        tree.limit(&limit2);
        Order limit3 = {3, Side::Buy, quantity_limit3, price3};
        tree.limit(&limit3);

        WHEN("a buy market order is submitted") {
            Order market = {4, Side::Sell, quantity_market, price2};
            tree.market(&market, [](UID) { });
            THEN("the limit_fill and market_fill functions should fire") {
                REQUIRE(nullptr != tree.best);
                REQUIRE(100 == tree.best->key);
                REQUIRE(20 == tree.volume_at(price1));
                REQUIRE(0 == tree.volume_at(price2));
                REQUIRE(0 == tree.volume_at(price3));
                REQUIRE(price1 == tree.last_best_price);
            }
            THEN("the order should have shares leftover") {
                REQUIRE(quantity_market - (quantity_limit2 + quantity_limit3) == market.quantity);
                REQUIRE(price2 == market.price);
            }
        }
    }
}

// ---------------------------------------------------------------------------
// MARK: clear
// ---------------------------------------------------------------------------

SCENARIO("clear a single limit from LimitTree") {
    Quantity quantity = 0x4545;
    Price price = 0xAABBCCDD00112233;
    GIVEN("a LimitTree with a single buy order") {
        auto tree = LimitTree<Side::Buy>();
        Order node = {1, Side::Buy, quantity, price};
        tree.limit(&node);
        WHEN("the tree is cleared") {
            tree.clear();
            THEN("the data structures are updated") {
                REQUIRE(0 == tree.limits.size());
                REQUIRE(0 == tree.volume_at(price));
                REQUIRE(0 == tree.count_at(price));
                REQUIRE(nullptr == tree.root);
                REQUIRE(nullptr == tree.best);
                REQUIRE(price == tree.last_best_price);
            }
        }
    }
    GIVEN("a LimitTree and with a single sell order") {
        auto tree = LimitTree<Side::Sell>();
        Order node = {1, Side::Sell, quantity, price};
        tree.limit(&node);
        WHEN("the tree is cleared") {
            tree.clear();
            THEN("the data structures are updated") {
                REQUIRE(0 == tree.limits.size());
                REQUIRE(0 == tree.volume_at(price));
                REQUIRE(0 == tree.count_at(price));
                REQUIRE(nullptr == tree.root);
                REQUIRE(nullptr == tree.best);
                REQUIRE(price == tree.last_best_price);
            }
        }
    }
    GIVEN("a LimitTree and with 2 single sell orders of the same price") {
        // just need to test sell because logic is side independent
        auto tree = LimitTree<Side::Sell>();
        Order node1 = {1, Side::Sell, quantity, price};
        tree.limit(&node1);
        Order node2 = {2, Side::Sell, quantity, price};
        tree.limit(&node2);
        WHEN("the tree is cleared") {
            tree.clear();
            THEN("the data structures are updated") {
                REQUIRE(0 == tree.limits.size());
                REQUIRE(0 == tree.volume_at(price));
                REQUIRE(0 == tree.count_at(price));
                REQUIRE(nullptr == tree.root);
                REQUIRE(nullptr == tree.best);
                REQUIRE(price == tree.last_best_price);
            }
        }
    }
}

SCENARIO("clear multiple limits from the tree") {
    Quantity quantity = 0x4545;
    Price price = 0xAABBCCDD00112233;
    auto priceHigher = price + 1;
    GIVEN("a LimitTree with 2 buy orders") {
        auto tree = LimitTree<Side::Buy>();
        Order node1 = {1, Side::Buy, quantity, priceHigher};
        tree.limit(&node1);
        Order node2 = {2, Side::Buy, quantity, price};
        tree.limit(&node2);
        WHEN("the tree is cleared") {
            tree.clear();
            THEN("the data structures are updated") {
                REQUIRE(0 == tree.limits.size());
                REQUIRE(0 == tree.volume_at(priceHigher));
                REQUIRE(0 == tree.volume_at(price));
                REQUIRE(0 == tree.count_at(price));
                REQUIRE(0 == tree.count_at(priceHigher));
                REQUIRE(nullptr == tree.root);
                REQUIRE(nullptr == tree.best);
                REQUIRE(priceHigher == tree.last_best_price);
            }
        }
    }
    GIVEN("a LimitTree with 2 sell orders") {
        auto tree = LimitTree<Side::Sell>();
        Order node1 = {1, Side::Sell, quantity, price};
        tree.limit(&node1);
        Order node2 = {2, Side::Buy, quantity, priceHigher};
        tree.limit(&node2);
        WHEN("the tree is cleared") {
            tree.clear();
            THEN("the data structures are updated") {
                REQUIRE(0 == tree.limits.size());
                REQUIRE(0 == tree.volume_at(priceHigher));
                REQUIRE(0 == tree.volume_at(price));
                REQUIRE(0 == tree.count_at(price));
                REQUIRE(0 == tree.count_at(priceHigher));
                REQUIRE(nullptr == tree.root);
                REQUIRE(nullptr == tree.best);
                REQUIRE(price == tree.last_best_price);
            }
        }
    }
}
