// A side of a limit order tree.
// Copyright 2019 Christian Kauten
//

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
    Size size = 0x4545;
    Price price = 0xAABBCCDD00112233;
    Timestamp arrival = 0xABABCDCDDEDEF0F0;
    GIVEN("a LimitTree and a single buy order") {
        auto tree = LimitTree<Side::Buy>();
        WHEN("the order is added") {
            Order node = {1, Side::Buy, size, price, arrival};
            tree.limit(&node);
            THEN("the order is recorded in the tree data structures") {
                REQUIRE(1 == tree.limits.size());
                REQUIRE(0 == tree.volume_at(price - 1));
                REQUIRE(size == tree.volume_at(price));
                REQUIRE(0 == tree.volume_at(price + 1));
                REQUIRE(0 == tree.size_at(price - 1));
                REQUIRE(1 == tree.size_at(price));
                REQUIRE(0 == tree.size_at(price + 1));
                REQUIRE(nullptr != tree.root);
                REQUIRE(nullptr != tree.best);
                REQUIRE(price == tree.best->key);
                REQUIRE(&node == tree.best->order_head);
                REQUIRE(&node == tree.best->order_tail);
                REQUIRE(tree.best == tree.root);
            }
        }
    }
    GIVEN("a LimitTree and a single sell order") {
        auto tree = LimitTree<Side::Sell>();
        WHEN("the order is added") {
            Order node = {1, Side::Sell, size, price, arrival};
            tree.limit(&node);
            THEN("the order is recorded in the tree data structures") {
                REQUIRE(1 == tree.limits.size());
                REQUIRE(0 == tree.volume_at(price - 1));
                REQUIRE(size == tree.volume_at(price));
                REQUIRE(0 == tree.volume_at(price + 1));
                REQUIRE(0 == tree.size_at(price - 1));
                REQUIRE(1 == tree.size_at(price));
                REQUIRE(0 == tree.size_at(price + 1));
                REQUIRE(nullptr != tree.root);
                REQUIRE(nullptr != tree.best);
                REQUIRE(price == tree.best->key);
                REQUIRE(&node == tree.best->order_head);
                REQUIRE(&node == tree.best->order_tail);
                REQUIRE(tree.best == tree.root);
            }
        }
    }
}

SCENARIO("add two orders to LimitTree (best first)") {
    Size size = 0x4545;
    Price price = 0xAABBCCDD00112233;
    auto priceHigher = price + 1;
    Timestamp arrival = 0xABABCDCDDEDEF0F0;
    GIVEN("a LimitTree and 2 buy orders") {
        auto tree = LimitTree<Side::Buy>();
        WHEN("the orders are added") {
            Order node1 = {1, Side::Buy, size, priceHigher, arrival};
            tree.limit(&node1);
            Order node2 = {2, Side::Buy, size, price, arrival};
            tree.limit(&node2);
            THEN("the orders are recorded in the tree data structures") {
                REQUIRE(2 == tree.limits.size());
                REQUIRE(size == tree.volume_at(priceHigher));
                REQUIRE(size == tree.volume_at(price));
                REQUIRE(1 == tree.size_at(price));
                REQUIRE(1 == tree.size_at(priceHigher));
                REQUIRE(nullptr != tree.root);
                REQUIRE(nullptr != tree.best);
                REQUIRE(tree.best == tree.root);
                REQUIRE(priceHigher == tree.best->key);
                REQUIRE(&node1 == tree.best->order_head);
                REQUIRE(&node1 == tree.best->order_tail);
            }
        }
    }
    GIVEN("a LimitTree and 2 sell orders") {
        auto tree = LimitTree<Side::Sell>();
        WHEN("the orders are added") {
            Order node1 = {1, Side::Sell, size, price, arrival};
            tree.limit(&node1);
            Order node2 = {2, Side::Buy, size, priceHigher, arrival};
            tree.limit(&node2);
            THEN("the orders are recorded in the tree data structures") {
                REQUIRE(2 == tree.limits.size());
                REQUIRE(size == tree.volume_at(priceHigher));
                REQUIRE(size == tree.volume_at(price));
                REQUIRE(1 == tree.size_at(price));
                REQUIRE(1 == tree.size_at(priceHigher));
                REQUIRE(nullptr != tree.root);
                REQUIRE(nullptr != tree.best);
                REQUIRE(tree.best == tree.root);
                REQUIRE(price == tree.best->key);
                REQUIRE(&node1 == tree.best->order_head);
                REQUIRE(&node1 == tree.best->order_tail);
            }
        }
    }
}

SCENARIO("add two orders to LimitTree (best last)") {
    Size size = 0x4545;
    Price price = 0xAABBCCDD00112233;
    auto priceHigher = price + 1;
    Timestamp arrival = 0xABABCDCDDEDEF0F0;
    GIVEN("a LimitTree and 2 buy orders") {
        auto tree = LimitTree<Side::Buy>();
        WHEN("the orders are added") {
            Order node1 = {1, Side::Buy, size, price, arrival};
            tree.limit(&node1);
            Order node2 = {2, Side::Buy, size, priceHigher, arrival};
            tree.limit(&node2);
            THEN("the orders are recorded in the tree data structures") {
                REQUIRE(2 == tree.limits.size());
                REQUIRE(size == tree.volume_at(priceHigher));
                REQUIRE(size == tree.volume_at(price));
                REQUIRE(1 == tree.size_at(price));
                REQUIRE(1 == tree.size_at(priceHigher));
                REQUIRE(nullptr != tree.root);
                REQUIRE(nullptr != tree.best);
                REQUIRE(tree.best != tree.root);
                REQUIRE(priceHigher == tree.best->key);
                REQUIRE(&node2 == tree.best->order_head);
                REQUIRE(&node2 == tree.best->order_tail);
            }
        }
    }
    GIVEN("a LimitTree and 2 sell orders") {
        auto tree = LimitTree<Side::Sell>();
        WHEN("the orders are added") {
            Order node1 = {1, Side::Sell, size, priceHigher, arrival};
            tree.limit(&node1);
            Order node2 = {2, Side::Buy, size, price, arrival};
            tree.limit(&node2);
            THEN("the orders are recorded in the tree data structures") {
                REQUIRE(2 == tree.limits.size());
                REQUIRE(size == tree.volume_at(priceHigher));
                REQUIRE(size == tree.volume_at(price));
                REQUIRE(1 == tree.size_at(price));
                REQUIRE(1 == tree.size_at(priceHigher));
                REQUIRE(nullptr != tree.root);
                REQUIRE(nullptr != tree.best);
                REQUIRE(tree.best != tree.root);
                REQUIRE(price == tree.best->key);
                REQUIRE(&node2 == tree.best->order_head);
                REQUIRE(&node2 == tree.best->order_tail);
            }
        }
    }
}

SCENARIO("add two orders to LimitTree (same price)") {
    Size size = 0x4545;
    Price price = 0xAABBCCDD00112233;
    Timestamp arrival = 0xABABCDCDDEDEF0F0;
    GIVEN("a LimitTree and 2 orders with the same price") {
        // only need to test buy because this logic is side independent
        auto tree = LimitTree<Side::Buy>();
        WHEN("the orders are added") {
            Order node1 = {1, Side::Buy, size, price, arrival};
            tree.limit(&node1);
            Order node2 = {2, Side::Buy, size, price, arrival};
            tree.limit(&node2);
            THEN("the orders are recorded in the tree data structures") {
                REQUIRE(1 == tree.limits.size());
                REQUIRE(2 * size == tree.volume_at(price));
                REQUIRE(nullptr != tree.root);
                REQUIRE(nullptr != tree.best);
                REQUIRE(2 == tree.size_at(price));
                REQUIRE(tree.best == tree.root);
                REQUIRE(price == tree.best->key);
                REQUIRE(&node1 == tree.best->order_head);
                REQUIRE(&node2 == tree.best->order_tail);
            }
        }
    }
}

// ---------------------------------------------------------------------------
// MARK: cancel
// ---------------------------------------------------------------------------

SCENARIO("cancel a single order from LimitTree") {
    Size size = 0x4545;
    Price price = 0xAABBCCDD00112233;
    Timestamp arrival = 0xABABCDCDDEDEF0F0;
    GIVEN("a LimitTree with a single buy order") {
        auto tree = LimitTree<Side::Buy>();
        Order node = {1, Side::Buy, size, price, arrival};
        tree.limit(&node);
        WHEN("the order is canceld") {
            tree.cancel(&node);
            THEN("the data structures are updated") {
                REQUIRE(0 == tree.limits.size());
                REQUIRE(0 == tree.volume_at(price));
                REQUIRE(0 == tree.size_at(price));
                REQUIRE(nullptr == tree.root);
                REQUIRE(nullptr == tree.best);
            }
        }
    }
    GIVEN("a LimitTree and with a single sell order") {
        auto tree = LimitTree<Side::Sell>();
        Order node = {1, Side::Sell, size, price, arrival};
        tree.limit(&node);
        WHEN("the order is canceld") {
            tree.cancel(&node);
            THEN("the data structures are updated") {
                REQUIRE(0 == tree.limits.size());
                REQUIRE(0 == tree.volume_at(price));
                REQUIRE(0 == tree.size_at(price));
                REQUIRE(nullptr == tree.root);
                REQUIRE(nullptr == tree.best);
            }
        }
    }
    GIVEN("a LimitTree and with 2 single sell orders of the same price") {
        // just need to test sell because logic is side independent
        auto tree = LimitTree<Side::Sell>();
        Order node1 = {1, Side::Sell, size, price, arrival};
        tree.limit(&node1);
        Order node2 = {2, Side::Sell, size, price, arrival};
        tree.limit(&node2);
        WHEN("the first order is canceld") {
            tree.cancel(&node1);
            THEN("the data structures are updated") {
                REQUIRE(1 == tree.limits.size());
                REQUIRE(size == tree.volume_at(price));
                REQUIRE(1 == tree.size_at(price));
                REQUIRE(tree.root == tree.best);
                REQUIRE(&node2 == tree.root->order_head);
                REQUIRE(&node2 == tree.root->order_tail);
            }
        }
        WHEN("the second order is canceld") {
            tree.cancel(&node2);
            THEN("the data structures are updated") {
                REQUIRE(1 == tree.limits.size());
                REQUIRE(size == tree.volume_at(price));
                REQUIRE(1 == tree.size_at(price));
                REQUIRE(tree.root == tree.best);
                REQUIRE(&node1 == tree.root->order_head);
                REQUIRE(&node1 == tree.root->order_tail);
            }
        }
    }
}

SCENARIO("cancel an order from LimitTree (best first)") {
    Size size = 0x4545;
    Price price = 0xAABBCCDD00112233;
    auto priceHigher = price + 1;
    Timestamp arrival = 0xABABCDCDDEDEF0F0;
    GIVEN("a LimitTree with 2 buy orders") {
        auto tree = LimitTree<Side::Buy>();
        Order node1 = {1, Side::Buy, size, priceHigher, arrival};
        tree.limit(&node1);
        Order node2 = {2, Side::Buy, size, price, arrival};
        tree.limit(&node2);
        WHEN("the best order is canceld") {
            tree.cancel(&node1);
            THEN("the data structures are updated") {
                REQUIRE(1 == tree.limits.size());
                REQUIRE(0 == tree.volume_at(priceHigher));
                REQUIRE(size == tree.volume_at(price));
                REQUIRE(1 == tree.size_at(price));
                REQUIRE(0 == tree.size_at(priceHigher));
                REQUIRE(nullptr != tree.root);
                REQUIRE(nullptr != tree.best);
                REQUIRE(tree.best == tree.root);
                REQUIRE(price == tree.best->key);
                REQUIRE(&node2 == tree.best->order_head);
                REQUIRE(&node2 == tree.best->order_tail);
            }
        }
        WHEN("the arbitrary order is canceld") {
            tree.cancel(&node2);
            THEN("the data structures are updated") {
                REQUIRE(1 == tree.limits.size());
                REQUIRE(size == tree.volume_at(priceHigher));
                REQUIRE(0 == tree.volume_at(price));
                REQUIRE(0 == tree.size_at(price));
                REQUIRE(1 == tree.size_at(priceHigher));
                REQUIRE(nullptr != tree.root);
                REQUIRE(nullptr != tree.best);
                REQUIRE(tree.best == tree.root);
                REQUIRE(priceHigher == tree.best->key);
                REQUIRE(&node1 == tree.best->order_head);
                REQUIRE(&node1 == tree.best->order_tail);
            }
        }
    }
    GIVEN("a LimitTree with 2 sell orders") {
        auto tree = LimitTree<Side::Sell>();
        Order node1 = {1, Side::Sell, size, price, arrival};
        tree.limit(&node1);
        Order node2 = {2, Side::Buy, size, priceHigher, arrival};
        tree.limit(&node2);
        WHEN("the best order is canceld") {
            tree.cancel(&node1);
            THEN("the data structures are updated") {
                REQUIRE(1 == tree.limits.size());
                REQUIRE(size == tree.volume_at(priceHigher));
                REQUIRE(0 == tree.volume_at(price));
                REQUIRE(0 == tree.size_at(price));
                REQUIRE(1 == tree.size_at(priceHigher));
                REQUIRE(nullptr != tree.root);
                REQUIRE(nullptr != tree.best);
                REQUIRE(tree.best == tree.root);
                REQUIRE(priceHigher == tree.best->key);
                REQUIRE(&node2 == tree.best->order_head);
                REQUIRE(&node2 == tree.best->order_tail);
            }
        }
        WHEN("the arbitrary order is canceld") {
            tree.cancel(&node2);
            THEN("the data structures are updated") {
                REQUIRE(1 == tree.limits.size());
                REQUIRE(0 == tree.volume_at(priceHigher));
                REQUIRE(size == tree.volume_at(price));
                REQUIRE(1 == tree.size_at(price));
                REQUIRE(0 == tree.size_at(priceHigher));
                REQUIRE(nullptr != tree.root);
                REQUIRE(nullptr != tree.best);
                REQUIRE(tree.best == tree.root);
                REQUIRE(price == tree.best->key);
                REQUIRE(&node1 == tree.best->order_head);
                REQUIRE(&node1 == tree.best->order_tail);
            }
        }
    }
}

SCENARIO("cancel an order from LimitTree (best last)") {
    Size size = 0x4545;
    Price price = 0xAABBCCDD00112233;
    auto priceHigher = price + 1;
    Timestamp arrival = 0xABABCDCDDEDEF0F0;
    GIVEN("a LimitTree with 2 buy orders") {
        auto tree = LimitTree<Side::Buy>();
        Order node1 = {1, Side::Buy, size, price, arrival};
        tree.limit(&node1);
        Order node2 = {2, Side::Buy, size, priceHigher, arrival};
        tree.limit(&node2);
        WHEN("the best order is canceld") {
            tree.cancel(&node2);
            THEN("the data structures are updated") {
                REQUIRE(1 == tree.limits.size());
                REQUIRE(0 == tree.volume_at(priceHigher));
                REQUIRE(size == tree.volume_at(price));
                REQUIRE(1 == tree.size_at(price));
                REQUIRE(0 == tree.size_at(priceHigher));
                REQUIRE(nullptr != tree.root);
                REQUIRE(nullptr != tree.best);
                REQUIRE(tree.best == tree.root);
                REQUIRE(price == tree.best->key);
                REQUIRE(&node1 == tree.best->order_head);
                REQUIRE(&node1 == tree.best->order_tail);
            }
        }
        WHEN("the arbitrary order is canceld") {
            tree.cancel(&node1);
            THEN("the data structures are updated") {
                REQUIRE(1 == tree.limits.size());
                REQUIRE(size == tree.volume_at(priceHigher));
                REQUIRE(0 == tree.volume_at(price));
                REQUIRE(0 == tree.size_at(price));
                REQUIRE(1 == tree.size_at(priceHigher));
                REQUIRE(nullptr != tree.root);
                REQUIRE(nullptr != tree.best);
                REQUIRE(tree.best == tree.root);
                REQUIRE(priceHigher == tree.best->key);
                REQUIRE(&node2 == tree.best->order_head);
                REQUIRE(&node2 == tree.best->order_tail);
            }
        }
    }
    GIVEN("a LimitTree with 2 sell orders") {
        auto tree = LimitTree<Side::Sell>();
        Order node1 = {1, Side::Sell, size, priceHigher, arrival};
        tree.limit(&node1);
        Order node2 = {2, Side::Buy, size, price, arrival};
        tree.limit(&node2);
        WHEN("the best order is canceld") {
            tree.cancel(&node2);
            THEN("the data structures are updated") {
                REQUIRE(1 == tree.limits.size());
                REQUIRE(size == tree.volume_at(priceHigher));
                REQUIRE(0 == tree.volume_at(price));
                REQUIRE(0 == tree.size_at(price));
                REQUIRE(1 == tree.size_at(priceHigher));
                REQUIRE(nullptr != tree.root);
                REQUIRE(nullptr != tree.best);
                REQUIRE(tree.best == tree.root);
                REQUIRE(priceHigher == tree.best->key);
                REQUIRE(&node1 == tree.best->order_head);
                REQUIRE(&node1 == tree.best->order_tail);
            }
        }
        WHEN("the arbitrary order is canceld") {
            tree.cancel(&node1);
            THEN("the data structures are updated") {
                REQUIRE(1 == tree.limits.size());
                REQUIRE(0 == tree.volume_at(priceHigher));
                REQUIRE(size == tree.volume_at(price));
                REQUIRE(1 == tree.size_at(price));
                REQUIRE(0 == tree.size_at(priceHigher));
                REQUIRE(nullptr != tree.root);
                REQUIRE(nullptr != tree.best);
                REQUIRE(tree.best == tree.root);
                REQUIRE(price == tree.best->key);
                REQUIRE(&node2 == tree.best->order_head);
                REQUIRE(&node2 == tree.best->order_tail);
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
        Size size = 100;
        Timestamp arrival = 1;
        WHEN("a buy market order is submitted") {
            Order market = {1, Side::Sell, size, 0, arrival};
            tree.market(&market, [](UID) { });
            THEN("the tree is empty") {
                REQUIRE(tree.root == nullptr);
                REQUIRE(tree.limits.size() == 0);
                REQUIRE(tree.best == nullptr);
                REQUIRE(tree.size == 0);
                REQUIRE(tree.volume == 0);
            }
        }
    }
}

SCENARIO("a market order is submitted to tree with a perfect match") {
    GIVEN("An order book with a limit order and a matched market order") {
        Size size = 100;
        Price price = 50;
        Timestamp arrival1 = 1;
        Timestamp arrival2 = 2;
        LimitTree<Side::Buy> tree;

        WHEN("a sell market order is matched to a buy limit order") {
            Order limit = {1, Side::Buy, size, price, arrival1};
            tree.limit(&limit);
            Order market = {2, Side::Sell, size, 0, arrival2};
            tree.market(&market, [](UID) { });
            THEN("the limit_fill and market_fill functions should fire") {
                REQUIRE(nullptr == tree.best);
                REQUIRE(0 == tree.volume_at(price));
            }
        }
    }
}

SCENARIO("a market order is submitted that is partially filled") {
    GIVEN("An order book with a limit order and a smaller market order") {
        Size size_limit = 100;
        Size size_market = 20;
        Price price = 50;
        Timestamp arrival1 = 1;
        Timestamp arrival2 = 2;
        LimitTree<Side::Buy> tree;
        Order limit = {1, Side::Buy, size_limit, price, arrival1};
        tree.limit(&limit);

        WHEN("a buy market order is submitted") {
            Order market = {2, Side::Sell, size_market, 0, arrival2};
            tree.market(&market, [](UID) { });
            THEN("the limit_fill and market_fill functions should fire") {
                REQUIRE(nullptr != tree.best);
                REQUIRE(price == tree.best->key);
                REQUIRE(size_limit - size_market == tree.volume_at(price));
            }
        }
    }
}

SCENARIO("a market order is submitted that spans several limit orders") {
    GIVEN("An order book with two limits and a market order that requires both") {
        Size size_limit1 = 40;
        Size size_limit2 = 20;
        Size size_market = 50;
        Price price = 100;
        Timestamp arrival1 = 1;
        Timestamp arrival2 = 2;
        Timestamp arrival3 = 3;
        LimitTree<Side::Buy> tree;
        Order limit1 = {1, Side::Buy, size_limit1, price, arrival1};
        tree.limit(&limit1);
        Order limit2 = {2, Side::Buy, size_limit2, price, arrival2};
        tree.limit(&limit2);

        WHEN("a buy market order is submitted") {
            Order market = {3, Side::Sell, size_market, 0, arrival3};
            tree.market(&market, [](UID) { });
            THEN("the limit_fill and market_fill functions should fire") {
                REQUIRE(nullptr != tree.best);
                REQUIRE(100 == tree.best->key);
                REQUIRE(size_limit1 + size_limit2 - size_market == tree.volume_at(price));
            }
        }
    }
}

SCENARIO("a market order is submitted that spans several limit orders and clears book") {
    GIVEN("An order book with two limits and a market order that requires both") {
        Size size_limit1 = 20;
        Size size_limit2 = 20;
        Size size_market = 50;
        Price price = 100;
        Timestamp arrival1 = 1;
        Timestamp arrival2 = 2;
        Timestamp arrival3 = 3;
        LimitTree<Side::Buy> tree;
        Order limit1 = {1, Side::Buy, size_limit1, price, arrival1};
        tree.limit(&limit1);
        Order limit2 = {2, Side::Buy, size_limit2, price, arrival2};
        tree.limit(&limit2);

        WHEN("a buy market order is submitted") {
            Order market = {3, Side::Sell, size_market, 0, arrival3};
            tree.market(&market, [](UID) { });
            THEN("the limit_fill and market_fill functions should fire") {
                REQUIRE(nullptr == tree.best);
                REQUIRE(0 == tree.volume_at(price));
            }
        }
    }
}

SCENARIO("a market order is submitted that spans several limit prices and clears book") {
    GIVEN("An order book with two limits and a market order that requires both") {
        Size size = 20;
        Price price1 = 101;
        Price price2 = 102;
        Timestamp arrival1 = 1;
        Timestamp arrival2 = 2;
        Timestamp arrival3 = 3;
        LimitTree<Side::Buy> tree;
        Order limit1 = {1, Side::Buy, size, price1, arrival1};
        tree.limit(&limit1);
        Order limit2 = {2, Side::Buy, size, price2, arrival2};
        tree.limit(&limit2);

        WHEN("a buy market order is submitted") {
            Order market = {3, Side::Sell, 2 * size, 0, arrival3};
            tree.market(&market, [](UID) { });
            THEN("the limit_fill and market_fill functions should fire") {
                REQUIRE(nullptr == tree.best);
                REQUIRE(0 == tree.volume_at(price1));
                REQUIRE(0 == tree.volume_at(price2));
            }
        }
    }
}

SCENARIO("a market order is submitted with a limit price") {
    GIVEN("A book with two limits and a market order with limit price") {
        Size size_limit1 = 20;
        Size size_limit2 = 20;
        Size size_market = 40;
        Price price1 = 100;
        Price price2 = 101;
        Timestamp arrival1 = 1;
        Timestamp arrival2 = 2;
        Timestamp arrival3 = 3;
        LimitTree<Side::Buy> tree;
        Order limit1 = {1, Side::Buy, size_limit1, price1, arrival1};
        tree.limit(&limit1);
        Order limit2 = {2, Side::Buy, size_limit2, price2, arrival2};
        tree.limit(&limit2);

        WHEN("a buy market order is submitted") {
            Order market = {3, Side::Sell, size_market, price2, arrival3};
            tree.market(&market, [](UID) { });
            THEN("the limit_fill and market_fill functions should fire") {
                REQUIRE(nullptr != tree.best);
                REQUIRE(100 == tree.best->key);
                REQUIRE(20 == tree.volume_at(price1));
                REQUIRE(0 == tree.volume_at(price2));
            }
            THEN("the order should have shares leftover") {
                REQUIRE(size_market - size_limit2 == market.size);
                REQUIRE(price2 == market.price);
            }
        }
    }
}

SCENARIO("a market order is submitted with a limit price that spans") {
    GIVEN("A book with two limits and a market order with limit price") {
        Size size_limit1 = 20;
        Size size_limit2 = 20;
        Size size_limit3 = 20;
        Size size_market = 60;
        Price price1 = 100;
        Price price2 = 101;
        Price price3 = 102;
        Timestamp arrival1 = 1;
        Timestamp arrival2 = 2;
        Timestamp arrival3 = 3;
        Timestamp arrival4 = 4;
        LimitTree<Side::Buy> tree;
        Order limit1 = {1, Side::Buy, size_limit1, price1, arrival1};
        tree.limit(&limit1);
        Order limit2 = {2, Side::Buy, size_limit2, price2, arrival2};
        tree.limit(&limit2);
        Order limit3 = {3, Side::Buy, size_limit3, price3, arrival3};
        tree.limit(&limit3);

        WHEN("a buy market order is submitted") {
            Order market = {4, Side::Sell, size_market, price2, arrival4};
            tree.market(&market, [](UID) { });
            THEN("the limit_fill and market_fill functions should fire") {
                REQUIRE(nullptr != tree.best);
                REQUIRE(100 == tree.best->key);
                REQUIRE(20 == tree.volume_at(price1));
                REQUIRE(0 == tree.volume_at(price2));
                REQUIRE(0 == tree.volume_at(price3));
            }
            THEN("the order should have shares leftover") {
                REQUIRE(size_market - (size_limit2 + size_limit3) == market.size);
                REQUIRE(price2 == market.price);
            }
        }
    }
}

// ---------------------------------------------------------------------------
// MARK: clear
// ---------------------------------------------------------------------------

SCENARIO("clear a single limit from LimitTree") {
    Size size = 0x4545;
    Price price = 0xAABBCCDD00112233;
    Timestamp arrival = 0xABABCDCDDEDEF0F0;
    GIVEN("a LimitTree with a single buy order") {
        auto tree = LimitTree<Side::Buy>();
        Order node = {1, Side::Buy, size, price, arrival};
        tree.limit(&node);
        WHEN("the tree is cleared") {
            tree.clear();
            THEN("the data structures are updated") {
                REQUIRE(0 == tree.limits.size());
                REQUIRE(0 == tree.volume_at(price));
                REQUIRE(0 == tree.size_at(price));
                REQUIRE(nullptr == tree.root);
                REQUIRE(nullptr == tree.best);
            }
        }
    }
    GIVEN("a LimitTree and with a single sell order") {
        auto tree = LimitTree<Side::Sell>();
        Order node = {1, Side::Sell, size, price, arrival};
        tree.limit(&node);
        WHEN("the tree is cleared") {
            tree.clear();
            THEN("the data structures are updated") {
                REQUIRE(0 == tree.limits.size());
                REQUIRE(0 == tree.volume_at(price));
                REQUIRE(0 == tree.size_at(price));
                REQUIRE(nullptr == tree.root);
                REQUIRE(nullptr == tree.best);
            }
        }
    }
    GIVEN("a LimitTree and with 2 single sell orders of the same price") {
        // just need to test sell because logic is side independent
        auto tree = LimitTree<Side::Sell>();
        Order node1 = {1, Side::Sell, size, price, arrival};
        tree.limit(&node1);
        Order node2 = {2, Side::Sell, size, price, arrival};
        tree.limit(&node2);
        WHEN("the tree is cleared") {
            tree.clear();
            THEN("the data structures are updated") {
                REQUIRE(0 == tree.limits.size());
                REQUIRE(0 == tree.volume_at(price));
                REQUIRE(0 == tree.size_at(price));
                REQUIRE(nullptr == tree.root);
                REQUIRE(nullptr == tree.best);
            }
        }
    }
}

SCENARIO("clear multiple limits from the tree") {
    Size size = 0x4545;
    Price price = 0xAABBCCDD00112233;
    auto priceHigher = price + 1;
    Timestamp arrival = 0xABABCDCDDEDEF0F0;
    GIVEN("a LimitTree with 2 buy orders") {
        auto tree = LimitTree<Side::Buy>();
        Order node1 = {1, Side::Buy, size, priceHigher, arrival};
        tree.limit(&node1);
        Order node2 = {2, Side::Buy, size, price, arrival};
        tree.limit(&node2);
        WHEN("the tree is cleared") {
            tree.clear();
            THEN("the data structures are updated") {
                REQUIRE(0 == tree.limits.size());
                REQUIRE(0 == tree.volume_at(priceHigher));
                REQUIRE(0 == tree.volume_at(price));
                REQUIRE(0 == tree.size_at(price));
                REQUIRE(0 == tree.size_at(priceHigher));
                REQUIRE(nullptr == tree.root);
                REQUIRE(nullptr == tree.best);
            }
        }
    }
    GIVEN("a LimitTree with 2 sell orders") {
        auto tree = LimitTree<Side::Sell>();
        Order node1 = {1, Side::Sell, size, price, arrival};
        tree.limit(&node1);
        Order node2 = {2, Side::Buy, size, priceHigher, arrival};
        tree.limit(&node2);
        WHEN("the tree is cleared") {
            tree.clear();
            THEN("the data structures are updated") {
                REQUIRE(0 == tree.limits.size());
                REQUIRE(0 == tree.volume_at(priceHigher));
                REQUIRE(0 == tree.volume_at(price));
                REQUIRE(0 == tree.size_at(price));
                REQUIRE(0 == tree.size_at(priceHigher));
                REQUIRE(nullptr == tree.root);
                REQUIRE(nullptr == tree.best);
            }
        }
    }
}
