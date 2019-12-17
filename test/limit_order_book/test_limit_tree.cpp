// A side of a limit order tree.
// Copyright 2019 Christian Kauten
//

#include "catch.hpp"
#include "limit_order_book/limit_tree.hpp"

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
            Account account;
            Order node = {&account, 1, Side::Buy, size, price, arrival};
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
            Account account;
            Order node = {&account, 1, Side::Sell, size, price, arrival};
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
            Account account;
            Order node1 = {&account, 1, Side::Buy, size, priceHigher, arrival};
            tree.limit(&node1);
            Order node2 = {&account, 2, Side::Buy, size, price, arrival};
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
            Account account;
            Order node1 = {&account, 1, Side::Sell, size, price, arrival};
            tree.limit(&node1);
            Order node2 = {&account, 2, Side::Buy, size, priceHigher, arrival};
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
            Account account;
            Order node1 = {&account, 1, Side::Buy, size, price, arrival};
            tree.limit(&node1);
            Order node2 = {&account, 2, Side::Buy, size, priceHigher, arrival};
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
            Account account;
            Order node1 = {&account, 1, Side::Sell, size, priceHigher, arrival};
            tree.limit(&node1);
            Order node2 = {&account, 2, Side::Buy, size, price, arrival};
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
            Account account;
            Order node1 = {&account, 1, Side::Buy, size, price, arrival};
            tree.limit(&node1);
            Order node2 = {&account, 2, Side::Buy, size, price, arrival};
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
// MARK: remove
// ---------------------------------------------------------------------------

SCENARIO("remove a single order from LimitTree") {
    Size size = 0x4545;
    Price price = 0xAABBCCDD00112233;
    Timestamp arrival = 0xABABCDCDDEDEF0F0;
    GIVEN("a LimitTree with a single buy order") {
        auto tree = LimitTree<Side::Buy>();
        Account account;
        Order node = {&account, 1, Side::Buy, size, price, arrival};
        tree.limit(&node);
        WHEN("the order is removed") {
            tree.remove(&node);
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
        Account account;
        Order node = {&account, 1, Side::Sell, size, price, arrival};
        tree.limit(&node);
        WHEN("the order is removed") {
            tree.remove(&node);
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
        Account account;
        Order node1 = {&account, 1, Side::Sell, size, price, arrival};
        tree.limit(&node1);
        Order node2 = {&account, 2, Side::Sell, size, price, arrival};
        tree.limit(&node2);
        WHEN("the first order is removed") {
            tree.remove(&node1);
            THEN("the data structures are updated") {
                REQUIRE(1 == tree.limits.size());
                REQUIRE(size == tree.volume_at(price));
                REQUIRE(1 == tree.size_at(price));
                REQUIRE(tree.root == tree.best);
                REQUIRE(&node2 == tree.root->order_head);
                REQUIRE(&node2 == tree.root->order_tail);
            }
        }
        WHEN("the second order is removed") {
            tree.remove(&node2);
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

SCENARIO("remove an order from LimitTree (best first)") {
    Size size = 0x4545;
    Price price = 0xAABBCCDD00112233;
    auto priceHigher = price + 1;
    Timestamp arrival = 0xABABCDCDDEDEF0F0;
    GIVEN("a LimitTree with 2 buy orders") {
        auto tree = LimitTree<Side::Buy>();
        Account account;
        Order node1 = {&account, 1, Side::Buy, size, priceHigher, arrival};
        tree.limit(&node1);
        Order node2 = {&account, 2, Side::Buy, size, price, arrival};
        tree.limit(&node2);
        WHEN("the best order is removed") {
            tree.remove(&node1);
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
        WHEN("the arbitrary order is removed") {
            tree.remove(&node2);
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
        Account account;
        Order node1 = {&account, 1, Side::Sell, size, price, arrival};
        tree.limit(&node1);
        Order node2 = {&account, 2, Side::Buy, size, priceHigher, arrival};
        tree.limit(&node2);
        WHEN("the best order is removed") {
            tree.remove(&node1);
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
        WHEN("the arbitrary order is removed") {
            tree.remove(&node2);
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

SCENARIO("remove an order from LimitTree (best last)") {
    Size size = 0x4545;
    Price price = 0xAABBCCDD00112233;
    auto priceHigher = price + 1;
    Timestamp arrival = 0xABABCDCDDEDEF0F0;
    GIVEN("a LimitTree with 2 buy orders") {
        auto tree = LimitTree<Side::Buy>();
        Account account;
        Order node1 = {&account, 1, Side::Buy, size, price, arrival};
        tree.limit(&node1);
        Order node2 = {&account, 2, Side::Buy, size, priceHigher, arrival};
        tree.limit(&node2);
        WHEN("the best order is removed") {
            tree.remove(&node2);
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
        WHEN("the arbitrary order is removed") {
            tree.remove(&node1);
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
        Account account;
        Order node1 = {&account, 1, Side::Sell, size, priceHigher, arrival};
        tree.limit(&node1);
        Order node2 = {&account, 2, Side::Buy, size, price, arrival};
        tree.limit(&node2);
        WHEN("the best order is removed") {
            tree.remove(&node2);
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
        WHEN("the arbitrary order is removed") {
            tree.remove(&node1);
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
        Account account;
        LimitTree<Side::Buy> tree;
        Size size = 100;
        Timestamp arrival = 1;
        WHEN("a buy market order is submitted") {
            Order market = {&account, 1, Side::Sell, size, 0, arrival};
            tree.market(&market, [](UID) { });
            THEN("the account should not be updated") {
                REQUIRE(account.order_count == 0);
                REQUIRE(account.volume == 0);
                REQUIRE(account.shares == 0);
                REQUIRE(account.capital == 0);
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
        Account maker;
        Account taker;
        LimitTree<Side::Buy> tree;

        WHEN("a sell market order is matched to a buy limit order") {
            Order limit = {&maker, 1, Side::Buy, size, price, arrival1};
            tree.limit(&limit);
            Order market = {&taker, 2, Side::Sell, size, 0, arrival2};
            tree.market(&market, [](UID) { });
            THEN("the limit_fill and market_fill functions should fire") {
                REQUIRE(nullptr == tree.best);
                REQUIRE(0 == tree.volume_at(price));
            }
            THEN("the maker account should be updated") {
                REQUIRE(maker.order_count == 0);
                REQUIRE(maker.volume == 0);
                REQUIRE(maker.shares == size);
                REQUIRE(maker.capital == -price * size);
            }
            THEN("the taker account should be updated") {
                REQUIRE(taker.order_count == 0);
                REQUIRE(taker.volume == 0);
                REQUIRE(taker.shares == -static_cast<Shares>(size));
                REQUIRE(taker.capital == price * size);
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
        Account maker;
        Account taker;
        LimitTree<Side::Buy> tree;
        Order limit = {&maker, 1, Side::Buy, size_limit, price, arrival1};
        tree.limit(&limit);

        WHEN("a buy market order is submitted") {
            Order market = {&taker, 2, Side::Sell, size_market, 0, arrival2};
            tree.market(&market, [](UID) { });
            THEN("the limit_fill and market_fill functions should fire") {
                REQUIRE(nullptr != tree.best);
                REQUIRE(price == tree.best->key);
                REQUIRE(size_limit - size_market == tree.volume_at(price));
            }
            THEN("the maker account should be updated") {
                REQUIRE(maker.order_count == 1);
                REQUIRE(maker.volume == size_limit - size_market);
                REQUIRE(maker.shares == size_market);
                REQUIRE(maker.capital == -price * size_market);
            }
            THEN("the taker account should be updated") {
                REQUIRE(taker.order_count == 0);
                REQUIRE(taker.volume == 0);
                REQUIRE(taker.shares == -static_cast<Shares>(size_market));
                REQUIRE(taker.capital == price * size_market);
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
        Account maker1;
        Account maker2;
        Account taker;
        LimitTree<Side::Buy> tree;
        Order limit1 = {&maker1, 1, Side::Buy, size_limit1, price, arrival1};
        tree.limit(&limit1);
        Order limit2 = {&maker2, 2, Side::Buy, size_limit2, price, arrival2};
        tree.limit(&limit2);

        WHEN("a buy market order is submitted") {
            Order market = {&taker, 3, Side::Sell, size_market, 0, arrival3};
            tree.market(&market, [](UID) { });
            THEN("the limit_fill and market_fill functions should fire") {
                REQUIRE(nullptr != tree.best);
                REQUIRE(100 == tree.best->key);
                REQUIRE(size_limit1 + size_limit2 - size_market == tree.volume_at(price));
            }
            THEN("the maker1 account should be updated") {
                REQUIRE(maker1.order_count == 0);
                REQUIRE(maker1.volume == 0);
                REQUIRE(maker1.shares == size_limit1);
                REQUIRE(maker1.capital == -price * size_limit1);
            }
            THEN("the maker2 account should be updated") {
                REQUIRE(maker2.order_count == 1);
                REQUIRE(maker2.volume == size_limit1 + size_limit2 - size_market);
                REQUIRE(maker2.shares == size_market - size_limit1);
                REQUIRE(maker2.capital == -price * (size_market - size_limit1));
            }
            THEN("the taker account should be updated") {
                REQUIRE(taker.order_count == 0);
                REQUIRE(taker.volume == 0);
                REQUIRE(taker.shares == -static_cast<Shares>(size_market));
                REQUIRE(taker.capital == price * size_market);
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

        Account maker1;
        Account maker2;
        Account taker;
        LimitTree<Side::Buy> tree;
        Order limit1 = {&maker1, 1, Side::Buy, size_limit1, price, arrival1};
        tree.limit(&limit1);
        Order limit2 = {&maker2, 2, Side::Buy, size_limit2, price, arrival2};
        tree.limit(&limit2);

        WHEN("a buy market order is submitted") {
            Order market = {&taker, 3, Side::Sell, size_market, 0, arrival3};
            tree.market(&market, [](UID) { });
            THEN("the limit_fill and market_fill functions should fire") {
                REQUIRE(nullptr == tree.best);
                REQUIRE(0 == tree.volume_at(price));
            }
            THEN("the maker1 account should be updated") {
                REQUIRE(maker1.order_count == 0);
                REQUIRE(maker1.volume == 0);
                REQUIRE(maker1.shares == size_limit1);
                REQUIRE(maker1.capital == -price * size_limit1);
            }
            THEN("the maker2 account should be updated") {
                REQUIRE(maker2.order_count == 0);
                REQUIRE(maker2.volume == 0);
                REQUIRE(maker2.shares == size_limit2);
                REQUIRE(maker2.capital == -price * size_limit2);
            }
            THEN("the taker account should be updated") {
                REQUIRE(taker.order_count == 0);
                REQUIRE(taker.volume == 0);
                REQUIRE(taker.shares == -static_cast<Shares>(size_limit1 + size_limit2));
                REQUIRE(taker.capital == price * (size_limit1 + size_limit2));
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

        Account maker1;
        Account maker2;
        Account taker;
        LimitTree<Side::Buy> tree;
        Order limit1 = {&maker1, 1, Side::Buy, size, price1, arrival1};
        tree.limit(&limit1);
        Order limit2 = {&maker2, 2, Side::Buy, size, price2, arrival2};
        tree.limit(&limit2);

        WHEN("a buy market order is submitted") {
            Order market = {&taker, 3, Side::Sell, 2 * size, 0, arrival3};
            tree.market(&market, [](UID) { });
            THEN("the limit_fill and market_fill functions should fire") {
                REQUIRE(nullptr == tree.best);
                REQUIRE(0 == tree.volume_at(price1));
                REQUIRE(0 == tree.volume_at(price2));
            }
            THEN("the maker1 account should be updated") {
                REQUIRE(maker1.order_count == 0);
                REQUIRE(maker1.volume == 0);
                REQUIRE(maker1.shares == size);
                REQUIRE(maker1.capital == -price1 * size);
            }
            THEN("the maker2 account should be updated") {
                REQUIRE(maker2.order_count == 0);
                REQUIRE(maker2.volume == 0);
                REQUIRE(maker2.shares == size);
                REQUIRE(maker2.capital == -price2 * size);
            }
            THEN("the taker account should be updated") {
                REQUIRE(taker.order_count == 0);
                REQUIRE(taker.volume == 0);
                REQUIRE(taker.shares == -static_cast<Shares>(2 * size));
                REQUIRE(taker.capital == size * (price1 + price2));
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
        Account maker1;
        Account maker2;
        Account taker;
        LimitTree<Side::Buy> tree;
        Order limit1 = {&maker1, 1, Side::Buy, size_limit1, price1, arrival1};
        tree.limit(&limit1);
        Order limit2 = {&maker2, 2, Side::Buy, size_limit2, price2, arrival2};
        tree.limit(&limit2);

        WHEN("a buy market order is submitted") {
            Order market = {&taker, 3, Side::Sell, size_market, price2, arrival3};
            tree.market(&market, [](UID) { });
            THEN("the limit_fill and market_fill functions should fire") {
                REQUIRE(nullptr != tree.best);
                REQUIRE(100 == tree.best->key);
                REQUIRE(20 == tree.volume_at(price1));
                REQUIRE(0 == tree.volume_at(price2));
            }
            THEN("the maker1 account should be updated") {
                REQUIRE(maker1.order_count == 1);
                REQUIRE(maker1.volume == size_limit1);
                REQUIRE(maker1.shares == 0);
                REQUIRE(maker1.capital == 0);
            }
            THEN("the maker2 account should be updated") {
                REQUIRE(maker2.order_count == 0);
                REQUIRE(maker2.volume == 0);
                REQUIRE(maker2.shares == size_limit2);
                REQUIRE(maker2.capital == -price2 * size_limit2);
            }
            THEN("the taker account should be updated") {
                REQUIRE(taker.order_count == 0);
                REQUIRE(taker.volume == 0);
                REQUIRE(taker.shares == -static_cast<Shares>(size_limit2));
                REQUIRE(taker.capital == price2 * size_limit2);
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
        Account maker1;
        Account maker2;
        Account maker3;
        Account taker;
        LimitTree<Side::Buy> tree;
        Order limit1 = {&maker1, 1, Side::Buy, size_limit1, price1, arrival1};
        tree.limit(&limit1);
        Order limit2 = {&maker2, 2, Side::Buy, size_limit2, price2, arrival2};
        tree.limit(&limit2);
        Order limit3 = {&maker3, 3, Side::Buy, size_limit3, price3, arrival3};
        tree.limit(&limit3);

        WHEN("a buy market order is submitted") {
            Order market = {&taker, 4, Side::Sell, size_market, price2, arrival4};
            tree.market(&market, [](UID) { });
            THEN("the limit_fill and market_fill functions should fire") {
                REQUIRE(nullptr != tree.best);
                REQUIRE(100 == tree.best->key);
                REQUIRE(20 == tree.volume_at(price1));
                REQUIRE(0 == tree.volume_at(price2));
                REQUIRE(0 == tree.volume_at(price3));
            }
            THEN("the maker1 account should be updated") {
                REQUIRE(maker1.order_count == 1);
                REQUIRE(maker1.volume == size_limit1);
                REQUIRE(maker1.shares == 0);
                REQUIRE(maker1.capital == 0);
            }
            THEN("the maker2 account should be updated") {
                REQUIRE(maker2.order_count == 0);
                REQUIRE(maker2.volume == 0);
                REQUIRE(maker2.shares == size_limit2);
                REQUIRE(maker2.capital == -price2 * size_limit2);
            }
            THEN("the maker3 account should be updated") {
                REQUIRE(maker3.order_count == 0);
                REQUIRE(maker3.volume == 0);
                REQUIRE(maker3.shares == size_limit3);
                REQUIRE(maker3.capital == -price3 * size_limit3);
            }
            THEN("the taker account should be updated") {
                REQUIRE(taker.order_count == 0);
                REQUIRE(taker.volume == 0);
                REQUIRE(taker.shares == -static_cast<Shares>(size_limit3 + size_limit2));
                REQUIRE(taker.capital == (price2 * size_limit2) + (price3 *size_limit3));
            }
            THEN("the order should have shares leftover") {
                REQUIRE(size_market - (size_limit2 + size_limit3) == market.size);
                REQUIRE(price2 == market.price);
            }
        }
    }
}
