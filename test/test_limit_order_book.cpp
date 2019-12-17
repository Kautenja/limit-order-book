// Test cases for the Limit Order Book (LOB) Object.
// Copyright 2019 Christian Kauten
//

#include "catch.hpp"
#include "limit_order_book.hpp"

using namespace LOB;

//
// MARK: Constructor
//

SCENARIO("initialize LimitOrderBook") {
    GIVEN("default parameters") {
        WHEN("a LimitOrderBook is initialized with no parameters") {
            REQUIRE_NOTHROW(new LimitOrderBook());
        }
    }
}

//
// MARK: limit
//

SCENARIO("send single order to LimitOrderBook") {
    GIVEN("an order book and a single sell order") {
        auto book = LimitOrderBook();
        Account account;
        auto side = Side::Sell;
        uint32_t size = 57;
        uint64_t price = 0xFEDCBA9876543210;
        uint64_t arrival = 0x1122334455667788;
        WHEN("the order is sent") {
            auto uid = book.limit(&account, side, size, price, arrival);
            THEN("order ID is returned and the order is recorded") {
                // the first order should have ID 1
                REQUIRE(1 == uid);
                REQUIRE(size == book.volume(price));
                REQUIRE(0 == book.volume(price - 1));
                REQUIRE(0 == book.volume(price + 1));
                REQUIRE(0 == book.best_buy());
                REQUIRE(price == book.best_sell());
            }
            THEN("the account information is correct") {
                REQUIRE(account.order_count == 1);
                REQUIRE(account.volume == size);
                REQUIRE(account.shares == 0);
                REQUIRE(account.capital == 0);
            }
        }
    }
    GIVEN("an order book and a single buy order") {
        auto book = LimitOrderBook();
        Account account;
        auto side = Side::Buy;
        uint32_t size = 57;
        uint64_t price = 0xFEDCBA9876543210;
        uint64_t arrival = 0x1122334455667788;
        WHEN("the order is sent") {
            auto uid = book.limit(&account, side, size, price, arrival);
            THEN("order ID is returned and the order is recorded") {
                // the first order should have ID 1
                REQUIRE(1 == uid);
                REQUIRE(size == book.volume(price));
                REQUIRE(0 == book.volume(price - 1));
                REQUIRE(0 == book.volume(price + 1));
                REQUIRE(price == book.best_buy());
                REQUIRE(0 == book.best_sell());
            }
            THEN("the account information is correct") {
                REQUIRE(account.order_count == 1);
                REQUIRE(account.volume == size);
                REQUIRE(account.shares == 0);
                REQUIRE(account.capital == 0);
            }
        }
    }
}

SCENARIO("send homogeneous orders to LimitOrderBook at same price") {
    GIVEN("an order book and a series of sell orders") {
        auto book = LimitOrderBook();
        Account account;
        auto side = Side::Sell;
        uint32_t sizeA = 50;
        uint32_t sizeB = 40;
        uint32_t sizeC = 30;
        uint64_t price = 0xFEDCBA9876543210;
        uint64_t arrivalA = 0;
        uint64_t arrivalB = 1;
        uint64_t arrivalC = 2;
        WHEN("the orders are sent") {
            auto uidA = book.limit(&account, side, sizeA, price, arrivalA);
            auto uidB = book.limit(&account, side, sizeB, price, arrivalB);
            auto uidC = book.limit(&account, side, sizeC, price, arrivalC);
            THEN("order ID is returned and the order is recorded") {
                REQUIRE(1 == uidA);
                REQUIRE(2 == uidB);
                REQUIRE(3 == uidC);
                REQUIRE(sizeA + sizeB + sizeC == book.volume(price));
                REQUIRE(0 == book.volume(price - 1));
                REQUIRE(0 == book.volume(price + 1));
                REQUIRE(0 == book.best_buy());
                REQUIRE(price == book.best_sell());
            }
            THEN("the account information is correct") {
                REQUIRE(account.order_count == 3);
                REQUIRE(account.volume == sizeA + sizeB + sizeC);
                REQUIRE(account.shares == 0);
                REQUIRE(account.capital == 0);
            }
        }
    }
    GIVEN("an order book and a series of buy orders") {
        auto book = LimitOrderBook();
        Account account;
        auto side = Side::Buy;
        uint32_t sizeA = 50;
        uint32_t sizeB = 40;
        uint32_t sizeC = 30;
        uint64_t price = 0xFEDCBA9876543210;
        uint64_t arrivalA = 0;
        uint64_t arrivalB = 1;
        uint64_t arrivalC = 2;
        WHEN("the orders are sent") {
            auto uidA = book.limit(&account, side, sizeA, price, arrivalA);
            auto uidB = book.limit(&account, side, sizeB, price, arrivalB);
            auto uidC = book.limit(&account, side, sizeC, price, arrivalC);
            THEN("order ID is returned and the order is recorded") {
                REQUIRE(1 == uidA);
                REQUIRE(2 == uidB);
                REQUIRE(3 == uidC);
                REQUIRE(sizeA + sizeB + sizeC == book.volume(price));
                REQUIRE(0 == book.volume(price - 1));
                REQUIRE(0 == book.volume(price + 1));
                REQUIRE(price == book.best_buy());
                REQUIRE(0 == book.best_sell());
            }
            THEN("the account information is correct") {
                REQUIRE(account.order_count == 3);
                REQUIRE(account.volume == sizeA + sizeB + sizeC);
                REQUIRE(account.shares == 0);
                REQUIRE(account.capital == 0);
            }
        }
    }
}

SCENARIO("send homogeneous orders to LimitOrderBook at different prices") {
    GIVEN("an order book and a series of sell orders") {
        auto book = LimitOrderBook();
        Account account;
        auto side = Side::Sell;
        uint32_t sizeA = 50;
        uint32_t sizeB = 40;
        uint32_t sizeC = 30;
        uint64_t priceA = 3253;
        uint64_t priceB = 3240;
        uint64_t priceC = 3245;
        uint64_t arrivalA = 0;
        uint64_t arrivalB = 1;
        uint64_t arrivalC = 2;
        WHEN("the orders are sent") {
            auto uidA = book.limit(&account, side, sizeA, priceA, arrivalA);
            auto uidB = book.limit(&account, side, sizeB, priceB, arrivalB);
            auto uidC = book.limit(&account, side, sizeC, priceC, arrivalC);
            THEN("order ID is returned and the order is recorded") {
                REQUIRE(1 == uidA);
                REQUIRE(2 == uidB);
                REQUIRE(3 == uidC);
                REQUIRE(sizeA == book.volume(priceA));
                REQUIRE(sizeB == book.volume(priceB));
                REQUIRE(sizeC == book.volume(priceC));
                REQUIRE(0 == book.best_buy());
                REQUIRE(priceB == book.best_sell());
            }
            THEN("the account information is correct") {
                REQUIRE(account.order_count == 3);
                REQUIRE(account.volume == sizeA + sizeB + sizeC);
                REQUIRE(account.shares == 0);
                REQUIRE(account.capital == 0);
            }
        }
    }
    GIVEN("an order book and a series of buy orders") {
        auto book = LimitOrderBook();
        Account account;
        auto side = Side::Buy;
        uint32_t sizeA = 50;
        uint32_t sizeB = 40;
        uint32_t sizeC = 30;
        uint64_t priceA = 3240;
        uint64_t priceB = 3253;
        uint64_t priceC = 3245;
        uint64_t arrivalA = 0;
        uint64_t arrivalB = 1;
        uint64_t arrivalC = 2;
        WHEN("the orders are sent") {
            auto uidA = book.limit(&account, side, sizeA, priceA, arrivalA);
            auto uidB = book.limit(&account, side, sizeB, priceB, arrivalB);
            auto uidC = book.limit(&account, side, sizeC, priceC, arrivalC);
            THEN("order ID is returned and the order is recorded") {
                REQUIRE(1 == uidA);
                REQUIRE(2 == uidB);
                REQUIRE(3 == uidC);
                REQUIRE(sizeA == book.volume(priceA));
                REQUIRE(sizeB == book.volume(priceB));
                REQUIRE(sizeC == book.volume(priceC));
                REQUIRE(priceB == book.best_buy());
                REQUIRE(0 == book.best_sell());
            }
            THEN("the account information is correct") {
                REQUIRE(account.order_count == 3);
                REQUIRE(account.volume == sizeA + sizeB + sizeC);
                REQUIRE(account.shares == 0);
                REQUIRE(account.capital == 0);
            }
        }
    }
}

SCENARIO("a limit order is submitted that crosses") {
    GIVEN("a book with 2 buy limit orders and a sell limit order") {
        auto book = LimitOrderBook();
        Account account1;
        Account account2;
        Account account3;
        auto side = Side::Buy;
        uint32_t size = 20;
        uint32_t sizeMarket = 40;
        uint64_t priceA = 100;
        uint64_t priceB = 101;
        uint64_t arrivalA = 0;
        uint64_t arrivalB = 1;
        uint64_t arrivalC = 2;
        auto uidA = book.limit(&account1, side, size, priceA, arrivalA);
        auto uidB = book.limit(&account2, side, size, priceB, arrivalB);
        WHEN("the sell limit order is sent") {
            auto uidC = book.limit(&account3, !side, sizeMarket, priceB, arrivalC);
            THEN("order ID is returned and the order is recorded") {
                REQUIRE(1 == uidA);
                REQUIRE(2 == uidB);
                REQUIRE(3 == uidC);
                REQUIRE(size == book.volume(priceA));
                REQUIRE(sizeMarket - size == book.volume(priceB));
                REQUIRE(priceA == book.best_buy());
                REQUIRE(priceB == book.best_sell());
            }
            THEN("the account1 information is correct") {
                REQUIRE(account1.order_count == 1);
                REQUIRE(account1.volume == size);
                REQUIRE(account1.shares == 0);
                REQUIRE(account1.capital == 0);
            }
            THEN("the account2 information is correct") {
                REQUIRE(account2.order_count == 0);
                REQUIRE(account2.volume == 0);
                REQUIRE(account2.shares == size);
                REQUIRE(account2.capital == -priceB * size);
            }
            THEN("the account3 information is correct") {
                REQUIRE(account3.order_count == 1);
                REQUIRE(account3.volume == sizeMarket - size);
                REQUIRE(account3.shares == -static_cast<Shares>(size));
                REQUIRE(account3.capital == priceB * size);
            }
        }
    }

    GIVEN("a book with 2 sell limit orders and a buy limit order") {
        auto book = LimitOrderBook();
        Account account1;
        Account account2;
        Account account3;
        auto side = Side::Sell;
        uint32_t size = 20;
        uint32_t sizeMarket = 40;
        uint64_t priceA = 101;
        uint64_t priceB = 100;
        uint64_t arrivalA = 0;
        uint64_t arrivalB = 1;
        uint64_t arrivalC = 2;
        auto uidA = book.limit(&account1, side, size, priceA, arrivalA);
        auto uidB = book.limit(&account2, side, size, priceB, arrivalB);
        WHEN("the sell limit order is sent") {
            auto uidC = book.limit(&account3, !side, sizeMarket, priceB, arrivalC);
            THEN("order ID is returned and the order is recorded") {
                REQUIRE(1 == uidA);
                REQUIRE(2 == uidB);
                REQUIRE(3 == uidC);
                REQUIRE(size == book.volume(priceA));
                REQUIRE(sizeMarket - size == book.volume(priceB));
                REQUIRE(priceA == book.best_sell());
                REQUIRE(priceB == book.best_buy());
            }
            THEN("the account1 information is correct") {
                REQUIRE(account1.order_count == 1);
                REQUIRE(account1.volume == size);
                REQUIRE(account1.shares == 0);
                REQUIRE(account1.capital == 0);
            }
            THEN("the account2 information is correct") {
                REQUIRE(account2.order_count == 0);
                REQUIRE(account2.volume == 0);
                REQUIRE(account2.shares == -static_cast<Shares>(size));
                REQUIRE(account2.capital == priceB * size);
            }
            THEN("the account3 information is correct") {
                REQUIRE(account3.order_count == 1);
                REQUIRE(account3.volume == sizeMarket - size);
                REQUIRE(account3.shares == size);
                REQUIRE(account3.capital == -priceB * size);
            }
        }
    }
}

//
// MARK: cancel
//

// limit removal

SCENARIO(R"(LOB
tree shape (single node):
*
)") {
    GIVEN("an order book and an ID for a single sell order") {
        auto book = LimitOrderBook();
        Account account;
        auto side = Side::Sell;
        uint32_t size = 50;
        uint64_t price = 3253;
        uint64_t arrival = 0x1122334455667788;
        auto uid = book.limit(&account, side, size, price, arrival);
        WHEN("the order is canceled") {
            book.cancel(uid);
            THEN("the limit is cleared") {
                REQUIRE(0 == book.volume(price));
                REQUIRE(0 == book.best_buy());
                REQUIRE(0 == book.best_sell());
            }
            THEN("the account information is correct") {
                REQUIRE(account.order_count == 0);
                REQUIRE(account.volume == 0);
                REQUIRE(account.shares == 0);
                REQUIRE(account.capital == 0);
            }
        }
        // tree integrity check
        WHEN("the order is duplicated, added, and canceled again") {
            book.cancel(uid);
            uint64_t uid1 = book.limit(&account, side, size, price, arrival + 1);
            book.cancel(uid1);
            THEN("the limit is cleared") {
                REQUIRE(0 == book.volume(price));
                REQUIRE(0 == book.best_buy());
                REQUIRE(0 == book.best_sell());
            }
            THEN("the account information is correct") {
                REQUIRE(account.order_count == 0);
                REQUIRE(account.volume == 0);
                REQUIRE(account.shares == 0);
                REQUIRE(account.capital == 0);
            }
        }
    }
    GIVEN("an order book and an ID for a single buy order") {
        auto book = LimitOrderBook();
        Account account;
        auto side = Side::Buy;
        uint32_t size = 50;
        uint64_t price = 3253;
        uint64_t arrival = 0x1122334455667788;
        auto uid = book.limit(&account, side, size, price, arrival);
        WHEN("the orders is canceled") {
            book.cancel(uid);
            THEN("the limit is cleared") {
                REQUIRE(0 == book.volume(price));
                REQUIRE(0 == book.best_buy());
                REQUIRE(0 == book.best_sell());
            }
            THEN("the account information is correct") {
                REQUIRE(account.order_count == 0);
                REQUIRE(account.volume == 0);
                REQUIRE(account.shares == 0);
                REQUIRE(account.capital == 0);
            }
        }
        // tree integrity check
        WHEN("the order is duplicated, added, and canceled again") {
            book.cancel(uid);
            uint64_t uid1 = book.limit(&account, side, size, price, arrival + 1);
            book.cancel(uid1);
            THEN("the limit is cleared") {
                REQUIRE(0 == book.volume(price));
                REQUIRE(0 == book.best_buy());
                REQUIRE(0 == book.best_sell());
            }
            THEN("the account information is correct") {
                REQUIRE(account.order_count == 0);
                REQUIRE(account.volume == 0);
                REQUIRE(account.shares == 0);
                REQUIRE(account.capital == 0);
            }
        }
    }
}

SCENARIO(R"(LOB
tree shape - left:
  1
 / \
0   2
)") {
    uint32_t size = 50;
    uint64_t prices[3] = {1, 2, 3};
    uint64_t arrivals[4] = {1, 2, 3, 4};
    GIVEN("an order book with V shaped limit tree (sell)") {
        auto book = LimitOrderBook();
        Account account;
        auto side = Side::Sell;
        // submit the MIDDLE order first (price-wise)
        book.limit(&account, side, size, prices[1], arrivals[0]);
        // the lowest price will be the left child
        auto uid = book.limit(&account, side, size, prices[0], arrivals[1]);
        // the highest price will be the right child
        book.limit(&account, side, size, prices[2], arrivals[2]);
        WHEN("the left order is canceled") {
            book.cancel(uid);
            THEN("the limit is cleared") {
                REQUIRE(0 == book.volume(prices[0]));
                REQUIRE(size == book.volume(prices[1]));
                REQUIRE(size == book.volume(prices[2]));
                REQUIRE(prices[1] == book.best_sell());
            }
            THEN("the account information is correct") {
                REQUIRE(account.order_count == 2);
                REQUIRE(account.volume == 2 * size);
                REQUIRE(account.shares == 0);
                REQUIRE(account.capital == 0);
            }
        }
        // tree integrity check
        WHEN("the left order is duplicated, added, and canceled again") {
            book.cancel(uid);
            uint64_t uid1 = book.limit(&account, side, size, prices[0], arrivals[3]);
            book.cancel(uid1);
            THEN("the limit is cleared") {
                REQUIRE(0 == book.volume(prices[0]));
                REQUIRE(size == book.volume(prices[1]));
                REQUIRE(size == book.volume(prices[2]));
                REQUIRE(prices[1] == book.best_sell());
            }
            THEN("the account information is correct") {
                REQUIRE(account.order_count == 2);
                REQUIRE(account.volume == 2 * size);
                REQUIRE(account.shares == 0);
                REQUIRE(account.capital == 0);
            }
        }
    }
    GIVEN("an order book with V shaped limit tree (buy)") {
        auto book = LimitOrderBook();
        Account account;
        auto side = Side::Buy;
        // submit the MIDDLE order first (price-wise)
        book.limit(&account, side, size, prices[1], arrivals[0]);
        // the lowest price will be the left child
        auto uid = book.limit(&account, side, size, prices[0], arrivals[1]);
        // the highest price will be the right child
        book.limit(&account, side, size, prices[2], arrivals[2]);
        WHEN("the left order is canceled") {
            book.cancel(uid);
            THEN("the limit is cleared") {
                REQUIRE(0 == book.volume(prices[0]));
                REQUIRE(size == book.volume(prices[1]));
                REQUIRE(size == book.volume(prices[2]));
                REQUIRE(prices[2] == book.best_buy());
            }
            THEN("the account information is correct") {
                REQUIRE(account.order_count == 2);
                REQUIRE(account.volume == 2 * size);
                REQUIRE(account.shares == 0);
                REQUIRE(account.capital == 0);
            }
        }
        // tree integrity check
        WHEN("the left order is duplicated, added, and canceled again") {
            book.cancel(uid);
            uint64_t uid1 = book.limit(&account, side, size, prices[0], arrivals[3]);
            book.cancel(uid1);
            THEN("the limit is cleared") {
                REQUIRE(0 == book.volume(prices[0]));
                REQUIRE(size == book.volume(prices[1]));
                REQUIRE(size == book.volume(prices[2]));
                REQUIRE(prices[2] == book.best_buy());
            }
            THEN("the account information is correct") {
                REQUIRE(account.order_count == 2);
                REQUIRE(account.volume == 2 * size);
                REQUIRE(account.shares == 0);
                REQUIRE(account.capital == 0);
            }
        }
    }
}

SCENARIO(R"(LOB
tree shape - right:
  1
 / \
0   2
)") {
    uint32_t size = 50;
    uint64_t prices[3] = {1, 2, 3};
    uint64_t arrivals[4] = {1, 2, 3, 4};
    GIVEN("an order book with V shaped limit tree (sell)") {
        auto book = LimitOrderBook();
        Account account;
        auto side = Side::Sell;
        // submit the MIDDLE order first (price-wise)
        book.limit(&account, side, size, prices[1], arrivals[0]);
        // the lowest price will be the left child
        book.limit(&account, side, size, prices[0], arrivals[1]);
        // the highest price will be the right child
        auto uid = book.limit(&account, side, size, prices[2], arrivals[2]);
        WHEN("the right order is canceled") {
            book.cancel(uid);
            THEN("the limit is cleared") {
                REQUIRE(size == book.volume(prices[0]));
                REQUIRE(size == book.volume(prices[1]));
                REQUIRE(0 == book.volume(prices[2]));
                REQUIRE(prices[0] == book.best_sell());
            }
            THEN("the account information is correct") {
                REQUIRE(account.order_count == 2);
                REQUIRE(account.volume == 2 * size);
                REQUIRE(account.shares == 0);
                REQUIRE(account.capital == 0);
            }
        }
        // tree integrity check
        WHEN("the right order is duplicated, added, and canceled again") {
            book.cancel(uid);
            uint64_t uid1 = book.limit(&account, side, size, prices[2], arrivals[3]);
            book.cancel(uid1);
            THEN("the limit is cleared") {
                REQUIRE(size == book.volume(prices[0]));
                REQUIRE(size == book.volume(prices[1]));
                REQUIRE(0 == book.volume(prices[2]));
                REQUIRE(prices[0] == book.best_sell());
            }
            THEN("the account information is correct") {
                REQUIRE(account.order_count == 2);
                REQUIRE(account.volume == 2 * size);
                REQUIRE(account.shares == 0);
                REQUIRE(account.capital == 0);
            }
        }
    }
    GIVEN("an order book with V shaped limit tree (buy)") {
        auto book = LimitOrderBook();
        Account account;
        auto side = Side::Buy;
        // submit the MIDDLE order first (price-wise)
        book.limit(&account, side, size, prices[1], arrivals[0]);
        // the lowest price will be the left child
        book.limit(&account, side, size, prices[0], arrivals[1]);
        // the highest price will be the right child
        auto uid = book.limit(&account, side, size, prices[2], arrivals[2]);
        WHEN("the right order is canceled") {
            book.cancel(uid);
            THEN("the limit is cleared") {
                REQUIRE(size == book.volume(prices[0]));
                REQUIRE(size == book.volume(prices[1]));
                REQUIRE(0 == book.volume(prices[2]));
                REQUIRE(prices[1] == book.best_buy());
            }
            THEN("the account information is correct") {
                REQUIRE(account.order_count == 2);
                REQUIRE(account.volume == 2 * size);
                REQUIRE(account.shares == 0);
                REQUIRE(account.capital == 0);
            }
        }
        // tree integrity check
        WHEN("the right order is duplicated, added, and canceled again") {
            book.cancel(uid);
            uint64_t uid1 = book.limit(&account, side, size, prices[2], arrivals[3]);
            book.cancel(uid1);
            THEN("the limit is cleared") {
                REQUIRE(size == book.volume(prices[0]));
                REQUIRE(size == book.volume(prices[1]));
                REQUIRE(0 == book.volume(prices[2]));
                REQUIRE(prices[1] == book.best_buy());
            }
            THEN("the account information is correct") {
                REQUIRE(account.order_count == 2);
                REQUIRE(account.volume == 2 * size);
                REQUIRE(account.shares == 0);
                REQUIRE(account.capital == 0);
            }
        }
    }
}

SCENARIO(R"(LOB
tree shape - root:
  1
 / \
0   2
)") {
    uint32_t size = 50;
    uint64_t prices[3] = {1, 2, 3};
    uint64_t arrivals[4] = {1, 2, 3, 4};
    GIVEN("an order book with V shaped limit tree (sell)") {
        auto book = LimitOrderBook();
        Account account;
        auto side = Side::Sell;
        // submit the MIDDLE order first (price-wise)
        auto uid = book.limit(&account, side, size, prices[1], arrivals[0]);
        // the lowest price will be the left child
        book.limit(&account, side, size, prices[0], arrivals[1]);
        // the highest price will be the right child
        book.limit(&account, side, size, prices[2], arrivals[2]);
        WHEN("the root order is canceled") {
            book.cancel(uid);
            THEN("the limit is cleared") {
                REQUIRE(size == book.volume(prices[0]));
                REQUIRE(0 == book.volume(prices[1]));
                REQUIRE(size == book.volume(prices[2]));
                REQUIRE(prices[0] == book.best_sell());
            }
            THEN("the account information is correct") {
                REQUIRE(account.order_count == 2);
                REQUIRE(account.volume == 2 * size);
                REQUIRE(account.shares == 0);
                REQUIRE(account.capital == 0);
            }
        }
        // tree integrity check
        WHEN("the root order is duplicated, added, and canceled again") {
            book.cancel(uid);
            uint64_t uid1 = book.limit(&account, side, size, prices[1], arrivals[3]);
            book.cancel(uid1);
            THEN("the limit is cleared") {
                REQUIRE(size == book.volume(prices[0]));
                REQUIRE(0 == book.volume(prices[1]));
                REQUIRE(size == book.volume(prices[2]));
                REQUIRE(prices[0] == book.best_sell());
            }
            THEN("the account information is correct") {
                REQUIRE(account.order_count == 2);
                REQUIRE(account.volume == 2 * size);
                REQUIRE(account.shares == 0);
                REQUIRE(account.capital == 0);
            }
        }
    }
    GIVEN("an order book with V shaped limit tree (buy)") {
        auto book = LimitOrderBook();
        Account account;
        auto side = Side::Buy;
        // submit the MIDDLE order first (price-wise)
        auto uid = book.limit(&account, side, size, prices[1], arrivals[0]);
        // the lowest price will be the left child
        book.limit(&account, side, size, prices[0], arrivals[1]);
        // the highest price will be the right child
        book.limit(&account, side, size, prices[2], arrivals[2]);
        WHEN("the root order is canceled") {
            book.cancel(uid);
            THEN("the limit is cleared") {
                REQUIRE(size == book.volume(prices[0]));
                REQUIRE(0 == book.volume(prices[1]));
                REQUIRE(size == book.volume(prices[2]));
                REQUIRE(prices[2] == book.best_buy());
            }
            THEN("the account information is correct") {
                REQUIRE(account.order_count == 2);
                REQUIRE(account.volume == 2 * size);
                REQUIRE(account.shares == 0);
                REQUIRE(account.capital == 0);
            }
        }
        // tree integrity check
        WHEN("the root order is duplicated, added, and canceled again") {
            book.cancel(uid);
            uint64_t uid1 = book.limit(&account, side, size, prices[1], arrivals[3]);
            book.cancel(uid1);
            THEN("the limit is cleared") {
                REQUIRE(size == book.volume(prices[0]));
                REQUIRE(0 == book.volume(prices[1]));
                REQUIRE(size == book.volume(prices[2]));
                REQUIRE(prices[2] == book.best_buy());
            }
            THEN("the account information is correct") {
                REQUIRE(account.order_count == 2);
                REQUIRE(account.volume == 2 * size);
                REQUIRE(account.shares == 0);
                REQUIRE(account.capital == 0);
            }
        }
    }
}

SCENARIO(R"(LOB
tree shape - root:
0
 \
  1
   \
    2
)") {
    uint32_t size = 50;
    uint64_t prices[3] = {1, 2, 3};
    uint64_t arrivals[4] = {1, 2, 3, 4};
    GIVEN("an order book with right leg shaped limit tree (sell)") {
        auto book = LimitOrderBook();
        Account account;
        auto side = Side::Sell;
        auto uid = book.limit(&account, side, size, prices[0], arrivals[0]);
        book.limit(&account, side, size, prices[1], arrivals[1]);
        book.limit(&account, side, size, prices[2], arrivals[2]);
        WHEN("the root order is canceled") {
            book.cancel(uid);
            THEN("the limit is cleared") {
                REQUIRE(0 == book.volume(prices[0]));
                REQUIRE(size == book.volume(prices[1]));
                REQUIRE(size == book.volume(prices[2]));
                REQUIRE(prices[1] == book.best_sell());
            }
            THEN("the account information is correct") {
                REQUIRE(account.order_count == 2);
                REQUIRE(account.volume == 2 * size);
                REQUIRE(account.shares == 0);
                REQUIRE(account.capital == 0);
            }
        }
        // tree integrity check
        WHEN("the root order is duplicated, added, and canceled again") {
            book.cancel(uid);
            uint64_t uid1 = book.limit(&account, side, size, prices[0], arrivals[3]);
            book.cancel(uid1);
            THEN("the limit is cleared") {
                REQUIRE(0 == book.volume(prices[0]));
                REQUIRE(size == book.volume(prices[1]));
                REQUIRE(size == book.volume(prices[2]));
                REQUIRE(prices[1] == book.best_sell());
            }
            THEN("the account information is correct") {
                REQUIRE(account.order_count == 2);
                REQUIRE(account.volume == 2 * size);
                REQUIRE(account.shares == 0);
                REQUIRE(account.capital == 0);
            }
        }
    }
    GIVEN("an order book with right leg shaped limit tree (buy)") {
        auto book = LimitOrderBook();
        Account account;
        auto side = Side::Buy;
        auto uid = book.limit(&account, side, size, prices[0], arrivals[0]);
        book.limit(&account, side, size, prices[1], arrivals[1]);
        book.limit(&account, side, size, prices[2], arrivals[2]);
        WHEN("the root order is canceled") {
            book.cancel(uid);
            THEN("the limit is cleared") {
                REQUIRE(0 == book.volume(prices[0]));
                REQUIRE(size == book.volume(prices[1]));
                REQUIRE(size == book.volume(prices[2]));
                REQUIRE(prices[2] == book.best_buy());
            }
            THEN("the account information is correct") {
                REQUIRE(account.order_count == 2);
                REQUIRE(account.volume == 2 * size);
                REQUIRE(account.shares == 0);
                REQUIRE(account.capital == 0);
            }
        }
        // tree integrity check
        WHEN("the root order is duplicated, added, and canceled again") {
            book.cancel(uid);
            uint64_t uid1 = book.limit(&account, side, size, prices[0], arrivals[3]);
            book.cancel(uid1);
            THEN("the limit is cleared") {
                REQUIRE(0 == book.volume(prices[0]));
                REQUIRE(size == book.volume(prices[1]));
                REQUIRE(size == book.volume(prices[2]));
                REQUIRE(prices[2] == book.best_buy());
            }
            THEN("the account information is correct") {
                REQUIRE(account.order_count == 2);
                REQUIRE(account.volume == 2 * size);
                REQUIRE(account.shares == 0);
                REQUIRE(account.capital == 0);
            }
        }
    }
}

SCENARIO(R"(LOB
tree shape - middle:
0
 \
  1
   \
    2
)") {
    uint32_t size = 50;
    uint64_t prices[3] = {1, 2, 3};
    uint64_t arrivals[4] = {1, 2, 3, 4};
    GIVEN("an order book with right leg shaped limit tree (sell)") {
        auto book = LimitOrderBook();
        Account account;
        auto side = Side::Sell;
        book.limit(&account, side, size, prices[0], arrivals[0]);
        auto uid = book.limit(&account, side, size, prices[1], arrivals[1]);
        book.limit(&account, side, size, prices[2], arrivals[2]);
        WHEN("the middle order is canceled") {
            book.cancel(uid);
            THEN("the limit is cleared") {
                REQUIRE(size == book.volume(prices[0]));
                REQUIRE(0 == book.volume(prices[1]));
                REQUIRE(size == book.volume(prices[2]));
                REQUIRE(prices[0] == book.best_sell());
            }
            THEN("the account information is correct") {
                REQUIRE(account.order_count == 2);
                REQUIRE(account.volume == 2 * size);
                REQUIRE(account.shares == 0);
                REQUIRE(account.capital == 0);
            }
        }
        // tree integrity check
        WHEN("the middle order is duplicated, added, and canceled again") {
            book.cancel(uid);
            uint64_t uid1 = book.limit(&account, side, size, prices[1], arrivals[3]);
            book.cancel(uid1);
            THEN("the limit is cleared") {
                REQUIRE(size == book.volume(prices[0]));
                REQUIRE(0 == book.volume(prices[1]));
                REQUIRE(size == book.volume(prices[2]));
                REQUIRE(prices[0] == book.best_sell());
            }
            THEN("the account information is correct") {
                REQUIRE(account.order_count == 2);
                REQUIRE(account.volume == 2 * size);
                REQUIRE(account.shares == 0);
                REQUIRE(account.capital == 0);
            }
        }
    }
    GIVEN("an order book with right leg shaped limit tree (buy)") {
        auto book = LimitOrderBook();
        Account account;
        auto side = Side::Buy;
        book.limit(&account, side, size, prices[0], arrivals[0]);
        auto uid = book.limit(&account, side, size, prices[1], arrivals[1]);
        book.limit(&account, side, size, prices[2], arrivals[2]);
        WHEN("the middle order is canceled") {
            book.cancel(uid);
            THEN("the limit is cleared") {
                REQUIRE(size == book.volume(prices[0]));
                REQUIRE(0 == book.volume(prices[1]));
                REQUIRE(size == book.volume(prices[2]));
                REQUIRE(prices[2] == book.best_buy());
            }
            THEN("the account information is correct") {
                REQUIRE(account.order_count == 2);
                REQUIRE(account.volume == 2 * size);
                REQUIRE(account.shares == 0);
                REQUIRE(account.capital == 0);
            }
        }
        // tree integrity check
        WHEN("the middle order is duplicated, added, and canceled again") {
            book.cancel(uid);
            uint64_t uid1 = book.limit(&account, side, size, prices[1], arrivals[3]);
            book.cancel(uid1);
            THEN("the limit is cleared") {
                REQUIRE(size == book.volume(prices[0]));
                REQUIRE(0 == book.volume(prices[1]));
                REQUIRE(size == book.volume(prices[2]));
                REQUIRE(prices[2] == book.best_buy());
            }
            THEN("the account information is correct") {
                REQUIRE(account.order_count == 2);
                REQUIRE(account.volume == 2 * size);
                REQUIRE(account.shares == 0);
                REQUIRE(account.capital == 0);
            }
        }
    }
}

SCENARIO(R"(LOB
tree shape - leaf:
0
 \
  1
   \
    2
)") {
    uint32_t size = 50;
    uint64_t prices[3] = {1, 2, 3};
    uint64_t arrivals[4] = {1, 2, 3, 4};
    GIVEN("an order book with right leg shaped limit tree (sell)") {
        auto book = LimitOrderBook();
        Account account;
        auto side = Side::Sell;
        book.limit(&account, side, size, prices[0], arrivals[0]);
        book.limit(&account, side, size, prices[1], arrivals[1]);
        auto uid = book.limit(&account, side, size, prices[2], arrivals[2]);
        WHEN("the leaf order is canceled") {
            book.cancel(uid);
            THEN("the limit is cleared") {
                REQUIRE(size == book.volume(prices[0]));
                REQUIRE(size == book.volume(prices[1]));
                REQUIRE(0 == book.volume(prices[2]));
                REQUIRE(prices[0] == book.best_sell());
            }
            THEN("the account information is correct") {
                REQUIRE(account.order_count == 2);
                REQUIRE(account.volume == 2 * size);
                REQUIRE(account.shares == 0);
                REQUIRE(account.capital == 0);
            }
        }
        // tree integrity check
        WHEN("the leaf order is duplicated, added, and canceled again") {
            book.cancel(uid);
            uint64_t uid1 = book.limit(&account, side, size, prices[2], arrivals[3]);
            book.cancel(uid1);
            THEN("the limit is cleared") {
                REQUIRE(size == book.volume(prices[0]));
                REQUIRE(size == book.volume(prices[1]));
                REQUIRE(0 == book.volume(prices[2]));
                REQUIRE(prices[0] == book.best_sell());
            }
            THEN("the account information is correct") {
                REQUIRE(account.order_count == 2);
                REQUIRE(account.volume == 2 * size);
                REQUIRE(account.shares == 0);
                REQUIRE(account.capital == 0);
            }
        }
    }
    GIVEN("an order book with right leg shaped limit tree (buy)") {
        auto book = LimitOrderBook();
        Account account;
        auto side = Side::Buy;
        book.limit(&account, side, size, prices[0], arrivals[0]);
        book.limit(&account, side, size, prices[1], arrivals[1]);
        auto uid = book.limit(&account, side, size, prices[2], arrivals[2]);
        WHEN("the leaf order is canceled") {
            book.cancel(uid);
            THEN("the limit is cleared") {
                REQUIRE(size == book.volume(prices[0]));
                REQUIRE(size == book.volume(prices[1]));
                REQUIRE(0 == book.volume(prices[2]));
                REQUIRE(prices[1] == book.best_buy());
            }
            THEN("the account information is correct") {
                REQUIRE(account.order_count == 2);
                REQUIRE(account.volume == 2 * size);
                REQUIRE(account.shares == 0);
                REQUIRE(account.capital == 0);
            }
        }
        // tree integrity check
        WHEN("the leaf order is duplicated, added, and canceled again") {
            book.cancel(uid);
            uint64_t uid1 = book.limit(&account, side, size, prices[2], arrivals[3]);
            book.cancel(uid1);
            THEN("the limit is cleared") {
                REQUIRE(size == book.volume(prices[0]));
                REQUIRE(size == book.volume(prices[1]));
                REQUIRE(0 == book.volume(prices[2]));
                REQUIRE(prices[1] == book.best_buy());
            }
            THEN("the account information is correct") {
                REQUIRE(account.order_count == 2);
                REQUIRE(account.volume == 2 * size);
                REQUIRE(account.shares == 0);
                REQUIRE(account.capital == 0);
            }
        }
    }
}

SCENARIO(R"(LOB
tree shape - root:
    2
   /
  1
 /
0
)") {
    uint32_t size = 50;
    uint64_t prices[3] = {1, 2, 3};
    uint64_t arrivals[4] = {1, 2, 3, 4};
    GIVEN("an order book with left leg shaped limit tree (sell)") {
        auto book = LimitOrderBook();
        Account account;
        auto side = Side::Sell;
        auto uid = book.limit(&account, side, size, prices[2], arrivals[0]);
        book.limit(&account, side, size, prices[1], arrivals[1]);
        book.limit(&account, side, size, prices[0], arrivals[2]);
        WHEN("the root order is canceled") {
            book.cancel(uid);
            THEN("the limit is cleared") {
                REQUIRE(size == book.volume(prices[0]));
                REQUIRE(size == book.volume(prices[1]));
                REQUIRE(0 == book.volume(prices[2]));
                REQUIRE(prices[0] == book.best_sell());
            }
            THEN("the account information is correct") {
                REQUIRE(account.order_count == 2);
                REQUIRE(account.volume == 2 * size);
                REQUIRE(account.shares == 0);
                REQUIRE(account.capital == 0);
            }
        }
        // tree integrity check
        WHEN("the root order is duplicated, added, and canceled again") {
            book.cancel(uid);
            uint64_t uid1 = book.limit(&account, side, size, prices[2], arrivals[3]);
            book.cancel(uid1);
            THEN("the limit is cleared") {
                REQUIRE(size == book.volume(prices[0]));
                REQUIRE(size == book.volume(prices[1]));
                REQUIRE(0 == book.volume(prices[2]));
                REQUIRE(prices[0] == book.best_sell());
            }
            THEN("the account information is correct") {
                REQUIRE(account.order_count == 2);
                REQUIRE(account.volume == 2 * size);
                REQUIRE(account.shares == 0);
                REQUIRE(account.capital == 0);
            }
        }
    }
    GIVEN("an order book with left leg shaped limit tree (buy)") {
        auto book = LimitOrderBook();
        Account account;
        auto side = Side::Buy;
        auto uid = book.limit(&account, side, size, prices[2], arrivals[0]);
        book.limit(&account, side, size, prices[1], arrivals[1]);
        book.limit(&account, side, size, prices[0], arrivals[2]);
        WHEN("the root order is canceled") {
            book.cancel(uid);
            THEN("the limit is cleared") {
                REQUIRE(size == book.volume(prices[0]));
                REQUIRE(size == book.volume(prices[1]));
                REQUIRE(0 == book.volume(prices[2]));
                REQUIRE(prices[1] == book.best_buy());
            }
            THEN("the account information is correct") {
                REQUIRE(account.order_count == 2);
                REQUIRE(account.volume == 2 * size);
                REQUIRE(account.shares == 0);
                REQUIRE(account.capital == 0);
            }
        }
        // tree integrity check
        WHEN("the root order is duplicated, added, and canceled again") {
            book.cancel(uid);
            uint64_t uid1 = book.limit(&account, side, size, prices[2], arrivals[3]);
            book.cancel(uid1);
            THEN("the limit is cleared") {
                REQUIRE(size == book.volume(prices[0]));
                REQUIRE(size == book.volume(prices[1]));
                REQUIRE(0 == book.volume(prices[2]));
                REQUIRE(prices[1] == book.best_buy());
            }
            THEN("the account information is correct") {
                REQUIRE(account.order_count == 2);
                REQUIRE(account.volume == 2 * size);
                REQUIRE(account.shares == 0);
                REQUIRE(account.capital == 0);
            }
        }
    }
}

SCENARIO(R"(LOB
tree shape - middle:
    2
   /
  1
 /
0
)") {
    uint32_t size = 50;
    uint64_t prices[3] = {1, 2, 3};
    uint64_t arrivals[4] = {1, 2, 3, 4};
    GIVEN("an order book with left leg shaped limit tree (sell)") {
        auto book = LimitOrderBook();
        Account account;
        auto side = Side::Sell;
        book.limit(&account, side, size, prices[2], arrivals[0]);
        auto uid = book.limit(&account, side, size, prices[1], arrivals[1]);
        book.limit(&account, side, size, prices[0], arrivals[2]);
        WHEN("the middle order is canceled") {
            book.cancel(uid);
            THEN("the limit is cleared") {
                REQUIRE(size == book.volume(prices[0]));
                REQUIRE(0 == book.volume(prices[1]));
                REQUIRE(size == book.volume(prices[2]));
                REQUIRE(prices[0] == book.best_sell());
            }
            THEN("the account information is correct") {
                REQUIRE(account.order_count == 2);
                REQUIRE(account.volume == 2 * size);
                REQUIRE(account.shares == 0);
                REQUIRE(account.capital == 0);
            }
        }
        // tree integrity check
        WHEN("the middle order is duplicated, added, and canceled again") {
            book.cancel(uid);
            uint64_t uid1 = book.limit(&account, side, size, prices[1], arrivals[3]);
            book.cancel(uid1);
            THEN("the limit is cleared") {
                REQUIRE(size == book.volume(prices[0]));
                REQUIRE(0 == book.volume(prices[1]));
                REQUIRE(size == book.volume(prices[2]));
                REQUIRE(prices[0] == book.best_sell());
            }
            THEN("the account information is correct") {
                REQUIRE(account.order_count == 2);
                REQUIRE(account.volume == 2 * size);
                REQUIRE(account.shares == 0);
                REQUIRE(account.capital == 0);
            }
        }
    }
    GIVEN("an order book with left leg shaped limit tree (buy)") {
        auto book = LimitOrderBook();
        Account account;
        auto side = Side::Buy;
        book.limit(&account, side, size, prices[2], arrivals[0]);
        auto uid = book.limit(&account, side, size, prices[1], arrivals[1]);
        book.limit(&account, side, size, prices[0], arrivals[2]);
        WHEN("the middle order is canceled") {
            book.cancel(uid);
            THEN("the limit is cleared") {
                REQUIRE(size == book.volume(prices[0]));
                REQUIRE(0 == book.volume(prices[1]));
                REQUIRE(size == book.volume(prices[2]));
                REQUIRE(prices[2] == book.best_buy());
            }
            THEN("the account information is correct") {
                REQUIRE(account.order_count == 2);
                REQUIRE(account.volume == 2 * size);
                REQUIRE(account.shares == 0);
                REQUIRE(account.capital == 0);
            }
        }
        // tree integrity check
        WHEN("the middle order is duplicated, added, and canceled again") {
            book.cancel(uid);
            uint64_t uid1 = book.limit(&account, side, size, prices[1], arrivals[3]);
            book.cancel(uid1);
            THEN("the limit is cleared") {
                REQUIRE(size == book.volume(prices[0]));
                REQUIRE(0 == book.volume(prices[1]));
                REQUIRE(size == book.volume(prices[2]));
                REQUIRE(prices[2] == book.best_buy());
            }
            THEN("the account information is correct") {
                REQUIRE(account.order_count == 2);
                REQUIRE(account.volume == 2 * size);
                REQUIRE(account.shares == 0);
                REQUIRE(account.capital == 0);
            }
        }
    }
}

SCENARIO(R"(LOB
tree shape - leaf:
    2
   /
  1
 /
0
)") {
    uint32_t size = 50;
    uint64_t prices[3] = {1, 2, 3};
    uint64_t arrivals[4] = {1, 2, 3, 4};
    GIVEN("an order book with left leg shaped limit tree (sell)") {
        auto book = LimitOrderBook();
        Account account;
        auto side = Side::Sell;
        book.limit(&account, side, size, prices[2], arrivals[0]);
        book.limit(&account, side, size, prices[1], arrivals[1]);
        auto uid = book.limit(&account, side, size, prices[0], arrivals[2]);
        WHEN("the leaf order is canceled") {
            book.cancel(uid);
            THEN("the limit is cleared") {
                REQUIRE(0 == book.volume(prices[0]));
                REQUIRE(size == book.volume(prices[1]));
                REQUIRE(size == book.volume(prices[2]));
                REQUIRE(prices[1] == book.best_sell());
            }
            THEN("the account information is correct") {
                REQUIRE(account.order_count == 2);
                REQUIRE(account.volume == 2 * size);
                REQUIRE(account.shares == 0);
                REQUIRE(account.capital == 0);
            }
        }
        // tree integrity check
        WHEN("the leaf order is duplicated, added, and canceled again") {
            book.cancel(uid);
            uint64_t uid1 = book.limit(&account, side, size, prices[0], arrivals[3]);
            book.cancel(uid1);
            THEN("the limit is cleared") {
                REQUIRE(0 == book.volume(prices[0]));
                REQUIRE(size == book.volume(prices[1]));
                REQUIRE(size == book.volume(prices[2]));
                REQUIRE(prices[1] == book.best_sell());
            }
            THEN("the account information is correct") {
                REQUIRE(account.order_count == 2);
                REQUIRE(account.volume == 2 * size);
                REQUIRE(account.shares == 0);
                REQUIRE(account.capital == 0);
            }
        }
    }
    GIVEN("an order book with left leg shaped limit tree (buy)") {
        auto book = LimitOrderBook();
        Account account;
        auto side = Side::Buy;
        book.limit(&account, side, size, prices[2], arrivals[0]);
        book.limit(&account, side, size, prices[1], arrivals[1]);
        auto uid = book.limit(&account, side, size, prices[0], arrivals[2]);
        WHEN("the leaf order is canceled") {
            book.cancel(uid);
            THEN("the limit is cleared") {
                REQUIRE(0 == book.volume(prices[0]));
                REQUIRE(size == book.volume(prices[1]));
                REQUIRE(size == book.volume(prices[2]));
                REQUIRE(prices[2] == book.best_buy());
            }
            THEN("the account information is correct") {
                REQUIRE(account.order_count == 2);
                REQUIRE(account.volume == 2 * size);
                REQUIRE(account.shares == 0);
                REQUIRE(account.capital == 0);
            }
        }
        // tree integrity check
        WHEN("the leaf order is duplicated, added, and canceled again") {
            book.cancel(uid);
            uint64_t uid1 = book.limit(&account, side, size, prices[0], arrivals[3]);
            book.cancel(uid1);
            THEN("the limit is cleared") {
                REQUIRE(0 == book.volume(prices[0]));
                REQUIRE(size == book.volume(prices[1]));
                REQUIRE(size == book.volume(prices[2]));
                REQUIRE(prices[2] == book.best_buy());
            }
            THEN("the account information is correct") {
                REQUIRE(account.order_count == 2);
                REQUIRE(account.volume == 2 * size);
                REQUIRE(account.shares == 0);
                REQUIRE(account.capital == 0);
            }
        }
    }
}

SCENARIO(R"(LOB
tree shape - root:
  2
 / \
1   4
   /
  3
)") {
    uint32_t size = 50;
    uint64_t prices[4] = {1, 2, 4, 3};
    uint64_t arrivals[5] = {1, 2, 3, 4, 5};
    GIVEN("an order book with right subtree with left branch--shaped limit tree (buy)") {
        auto book = LimitOrderBook();
        Account account;
        auto side = Side::Buy;
        // submit the MIDDLE order first (price-wise)
        auto uid = book.limit(&account, side, size, prices[1], arrivals[0]);
        // the lowest price will be the left child
        book.limit(&account, side, size, prices[0], arrivals[1]);
        // the highest price will be the right child
        book.limit(&account, side, size, prices[2], arrivals[2]);
        // the last price will be the left branch of the right child
        book.limit(&account, side, size, prices[3], arrivals[3]);
        WHEN("the root order is canceled") {
            book.cancel(uid);
            THEN("the limit is cleared") {
                REQUIRE(size == book.volume(prices[0]));
                REQUIRE(0 == book.volume(prices[1]));
                REQUIRE(size == book.volume(prices[2]));
                REQUIRE(size == book.volume(prices[3]));
                REQUIRE(prices[2] == book.best_buy());
            }
            THEN("the account information is correct") {
                REQUIRE(account.order_count == 3);
                REQUIRE(account.volume == 3 * size);
                REQUIRE(account.shares == 0);
                REQUIRE(account.capital == 0);
            }
        }
        // tree integrity check
        WHEN("the root order is duplicated, added, and canceled again") {
            book.cancel(uid);
            uint64_t uid1 = book.limit(&account, side, size, prices[1], arrivals[4]);
            book.cancel(uid1);
            THEN("the limit is cleared") {
                REQUIRE(size == book.volume(prices[0]));
                REQUIRE(0 == book.volume(prices[1]));
                REQUIRE(size == book.volume(prices[2]));
                REQUIRE(size == book.volume(prices[3]));
                REQUIRE(prices[2] == book.best_buy());
            }
            THEN("the account information is correct") {
                REQUIRE(account.order_count == 3);
                REQUIRE(account.volume == 3 * size);
                REQUIRE(account.shares == 0);
                REQUIRE(account.capital == 0);
            }
        }
    }
}

SCENARIO(R"(LOB
tree shape - root:
  2
 / \
1   5
   /
  3
   \
    4
)") {
    uint32_t size = 50;
    uint64_t prices[5] = {1, 2, 5, 3, 4};
    uint64_t arrivals[6] = {1, 2, 3, 4, 5, 6};
    GIVEN("an order book with right subtree with left branch and terminal right child--shaped limit tree (buy)") {
        auto book = LimitOrderBook();
        Account account;
        auto side = Side::Buy;
        // submit the MIDDLE order first (price-wise)
        auto uid = book.limit(&account, side, size, prices[1], arrivals[0]);
        // the lowest price will be the left child
        book.limit(&account, side, size, prices[0], arrivals[1]);
        // the highest price will be the right child
        book.limit(&account, side, size, prices[2], arrivals[2]);
        // the last price will be the left branch of the right child
        book.limit(&account, side, size, prices[3], arrivals[3]);
        // the last price will be the left branch of the right child
        book.limit(&account, side, size, prices[4], arrivals[4]);
        WHEN("the root order is canceled") {
            book.cancel(uid);
            THEN("the limit is cleared") {
                REQUIRE(size == book.volume(prices[0]));
                REQUIRE(0 == book.volume(prices[1]));
                REQUIRE(size == book.volume(prices[2]));
                REQUIRE(size == book.volume(prices[3]));
                REQUIRE(size == book.volume(prices[4]));
                REQUIRE(prices[2] == book.best_buy());
            }
            THEN("the account information is correct") {
                REQUIRE(account.order_count == 4);
                REQUIRE(account.volume == 4 * size);
                REQUIRE(account.shares == 0);
                REQUIRE(account.capital == 0);
            }
        }
        // tree integrity check
        WHEN("the root order is duplicated, added, and canceled again") {
            book.cancel(uid);
            uint64_t uid1 = book.limit(&account, side, size, prices[1], arrivals[5]);
            book.cancel(uid1);
            THEN("the limit is cleared") {
                REQUIRE(size == book.volume(prices[0]));
                REQUIRE(0 == book.volume(prices[1]));
                REQUIRE(size == book.volume(prices[2]));
                REQUIRE(size == book.volume(prices[3]));
                REQUIRE(size == book.volume(prices[4]));
                REQUIRE(prices[2] == book.best_buy());
            }
            THEN("the account information is correct") {
                REQUIRE(account.order_count == 4);
                REQUIRE(account.volume == 4 * size);
                REQUIRE(account.shares == 0);
                REQUIRE(account.capital == 0);
            }
        }
    }
}

SCENARIO(R"(LOB
tree shape - node 4:
1
 \
  4
 /
2
 \
  3
)") {
    uint32_t size = 50;
    uint64_t prices[4] = {1, 4, 2, 3};
    uint64_t arrivals[5] = {1, 2, 3, 4, 5};
    GIVEN("an order book with right zigzag--shaped limit tree (buy)") {
        auto book = LimitOrderBook();
        Account account;
        auto side = Side::Buy;
        book.limit(&account, side, size, prices[0], arrivals[0]);
        auto uid = book.limit(&account, side, size, prices[1], arrivals[1]);
        book.limit(&account, side, size, prices[2], arrivals[2]);
        book.limit(&account, side, size, prices[3], arrivals[3]);
        WHEN("the root-child order is canceled") {
            book.cancel(uid);
            THEN("the limit is cleared") {
                REQUIRE(size == book.volume(prices[0]));
                REQUIRE(0 == book.volume(prices[1]));
                REQUIRE(size == book.volume(prices[2]));
                REQUIRE(size == book.volume(prices[3]));
                REQUIRE(prices[3] == book.best_buy());
            }
            THEN("the account information is correct") {
                REQUIRE(account.order_count == 3);
                REQUIRE(account.volume == 3 * size);
                REQUIRE(account.shares == 0);
                REQUIRE(account.capital == 0);
            }
        }
        // tree integrity check
        WHEN("the root-child order is duplicated, added, and canceled again") {
            book.cancel(uid);
            uint64_t uid1 = book.limit(&account, side, size, prices[1], arrivals[4]);
            book.cancel(uid1);
            THEN("the limit is cleared") {
                REQUIRE(size == book.volume(prices[0]));
                REQUIRE(0 == book.volume(prices[1]));
                REQUIRE(size == book.volume(prices[2]));
                REQUIRE(size == book.volume(prices[3]));
                REQUIRE(prices[3] == book.best_buy());
            }
            THEN("the account information is correct") {
                REQUIRE(account.order_count == 3);
                REQUIRE(account.volume == 3 * size);
                REQUIRE(account.shares == 0);
                REQUIRE(account.capital == 0);
            }
        }
    }
}

SCENARIO(R"(LOB
tree shape - node 1:
  4
 /
1
 \
  3
 /
2
)") {
    uint32_t size = 50;
    uint64_t prices[4] = {4, 1, 3, 2};
    uint64_t arrivals[5] = {1, 2, 3, 4, 5};
    GIVEN("an order book with left zigzag--shaped limit tree (sell)") {
        auto book = LimitOrderBook();
        Account account;
        auto side = Side::Sell;
        book.limit(&account, side, size, prices[0], arrivals[0]);
        auto uid = book.limit(&account, side, size, prices[1], arrivals[1]);
        book.limit(&account, side, size, prices[2], arrivals[2]);
        book.limit(&account, side, size, prices[3], arrivals[3]);
        WHEN("the root-child order is canceled") {
            book.cancel(uid);
            THEN("the limit is cleared") {
                REQUIRE(size == book.volume(prices[0]));
                REQUIRE(0 == book.volume(prices[1]));
                REQUIRE(size == book.volume(prices[2]));
                REQUIRE(size == book.volume(prices[3]));
                REQUIRE(prices[3] == book.best_sell());
            }
            THEN("the account information is correct") {
                REQUIRE(account.order_count == 3);
                REQUIRE(account.volume == 3 * size);
                REQUIRE(account.shares == 0);
                REQUIRE(account.capital == 0);
            }
        }
        // tree integrity check
        WHEN("the root-child order is duplicated, added, and canceled again") {
            book.cancel(uid);
            uint64_t uid1 = book.limit(&account, side, size, prices[1], arrivals[4]);
            book.cancel(uid1);
            THEN("the limit is cleared") {
                REQUIRE(size == book.volume(prices[0]));
                REQUIRE(0 == book.volume(prices[1]));
                REQUIRE(size == book.volume(prices[2]));
                REQUIRE(size == book.volume(prices[3]));
                REQUIRE(prices[3] == book.best_sell());
            }
            THEN("the account information is correct") {
                REQUIRE(account.order_count == 3);
                REQUIRE(account.volume == 3 * size);
                REQUIRE(account.shares == 0);
                REQUIRE(account.capital == 0);
            }
        }
    }
}

// order removal

SCENARIO("cancel first order in a Limit queue of orders") {
    GIVEN("an order book and a Limit queue of sell orders") {
        auto book = LimitOrderBook();
        Account account;
        auto side = Side::Sell;
        uint32_t sizeA = 50;
        uint32_t sizeB = 40;
        uint32_t sizeC = 30;
        uint64_t price = 3253;
        uint64_t arrivalA = 0;
        uint64_t arrivalB = 1;
        uint64_t arrivalC = 2;
        auto uid = book.limit(&account, side, sizeA, price, arrivalA);
        book.limit(&account, side, sizeB, price, arrivalB);
        book.limit(&account, side, sizeC, price, arrivalC);
        WHEN("the first order is canceled") {
            book.cancel(uid);
            THEN("order ID is returned and the order is recorded") {
                REQUIRE(sizeB + sizeC == book.volume(price));
                REQUIRE(0 == book.best_buy());
                REQUIRE(price == book.best_sell());
            }
            THEN("the account information is correct") {
                REQUIRE(account.order_count == 2);
                REQUIRE(account.volume == sizeB + sizeC);
                REQUIRE(account.shares == 0);
                REQUIRE(account.capital == 0);
            }
        }
    }
    GIVEN("an order book and a Limit queue of buy orders") {
        auto book = LimitOrderBook();
        Account account;
        auto side = Side::Buy;
        uint32_t sizeA = 50;
        uint32_t sizeB = 40;
        uint32_t sizeC = 30;
        uint64_t price = 3253;
        uint64_t arrivalA = 0;
        uint64_t arrivalB = 1;
        uint64_t arrivalC = 2;
        auto uid = book.limit(&account, side, sizeA, price, arrivalA);
        book.limit(&account, side, sizeB, price, arrivalB);
        book.limit(&account, side, sizeC, price, arrivalC);
        WHEN("the first order is canceled") {
            book.cancel(uid);
            THEN("order ID is returned and the order is recorded") {
                REQUIRE(sizeB + sizeC == book.volume(price));
                REQUIRE(price == book.best_buy());
                REQUIRE(0 == book.best_sell());
            }
            THEN("the account information is correct") {
                REQUIRE(account.order_count == 2);
                REQUIRE(account.volume == sizeB + sizeC);
                REQUIRE(account.shares == 0);
                REQUIRE(account.capital == 0);
            }
        }
    }
}

SCENARIO("cancel middle order in a Limit queue of orders") {
    GIVEN("an order book and a Limit queue of sell orders") {
        auto book = LimitOrderBook();
        Account account;
        auto side = Side::Sell;
        uint32_t sizeA = 50;
        uint32_t sizeB = 40;
        uint32_t sizeC = 30;
        uint64_t price = 3253;
        uint64_t arrivalA = 0;
        uint64_t arrivalB = 1;
        uint64_t arrivalC = 2;
        book.limit(&account, side, sizeA, price, arrivalA);
        auto uid = book.limit(&account, side, sizeB, price, arrivalB);
        book.limit(&account, side, sizeC, price, arrivalC);
        WHEN("the middle order is canceled") {
            book.cancel(uid);
            THEN("order ID is returned and the order is recorded") {
                REQUIRE(sizeA + sizeC == book.volume(price));
                REQUIRE(0 == book.best_buy());
                REQUIRE(price == book.best_sell());
            }
            THEN("the account information is correct") {
                REQUIRE(account.order_count == 2);
                REQUIRE(account.volume == sizeA + sizeC);
                REQUIRE(account.shares == 0);
                REQUIRE(account.capital == 0);
            }
        }
    }
    GIVEN("an order book and a Limit queue of buy orders") {
        auto book = LimitOrderBook();
        Account account;
        auto side = Side::Buy;
        uint32_t sizeA = 50;
        uint32_t sizeB = 40;
        uint32_t sizeC = 30;
        uint64_t price = 3253;
        uint64_t arrivalA = 0;
        uint64_t arrivalB = 1;
        uint64_t arrivalC = 2;
        book.limit(&account, side, sizeA, price, arrivalA);
        auto uid = book.limit(&account, side, sizeB, price, arrivalB);
        book.limit(&account, side, sizeC, price, arrivalC);
        WHEN("the middle order is canceled") {
            book.cancel(uid);
            THEN("order ID is returned and the order is recorded") {
                REQUIRE(sizeA + sizeC == book.volume(price));
                REQUIRE(price == book.best_buy());
                REQUIRE(0 == book.best_sell());
            }
            THEN("the account information is correct") {
                REQUIRE(account.order_count == 2);
                REQUIRE(account.volume == sizeA + sizeC);
                REQUIRE(account.shares == 0);
                REQUIRE(account.capital == 0);
            }
        }
    }
}

SCENARIO("cancel last order in a Limit queue of orders") {
    GIVEN("an order book and a Limit queue of sell orders") {
        auto book = LimitOrderBook();
        Account account;
        auto side = Side::Sell;
        uint32_t sizeA = 50;
        uint32_t sizeB = 40;
        uint32_t sizeC = 30;
        uint64_t price = 3253;
        uint64_t arrivalA = 0;
        uint64_t arrivalB = 1;
        uint64_t arrivalC = 2;
        book.limit(&account, side, sizeA, price, arrivalA);
        book.limit(&account, side, sizeB, price, arrivalB);
        auto uid = book.limit(&account, side, sizeC, price, arrivalC);
        WHEN("the last order is canceled") {
            book.cancel(uid);
            THEN("order ID is returned and the order is recorded") {
                REQUIRE(sizeA + sizeB == book.volume(price));
                REQUIRE(0 == book.best_buy());
                REQUIRE(price == book.best_sell());
            }
            THEN("the account information is correct") {
                REQUIRE(account.order_count == 2);
                REQUIRE(account.volume == sizeA + sizeB);
                REQUIRE(account.shares == 0);
                REQUIRE(account.capital == 0);
            }
        }
    }
    GIVEN("an order book and a Limit queue of buy orders") {
        auto book = LimitOrderBook();
        Account account;
        auto side = Side::Buy;
        uint32_t sizeA = 50;
        uint32_t sizeB = 40;
        uint32_t sizeC = 30;
        uint64_t price = 3253;
        uint64_t arrivalA = 0;
        uint64_t arrivalB = 1;
        uint64_t arrivalC = 2;
        book.limit(&account, side, sizeA, price, arrivalA);
        book.limit(&account, side, sizeB, price, arrivalB);
        auto uid = book.limit(&account, side, sizeC, price, arrivalC);
        WHEN("the last order is canceled") {
            book.cancel(uid);
            THEN("order ID is returned and the order is recorded") {
                REQUIRE(sizeA + sizeB == book.volume(price));
                REQUIRE(price == book.best_buy());
                REQUIRE(0 == book.best_sell());
            }
            THEN("the account information is correct") {
                REQUIRE(account.order_count == 2);
                REQUIRE(account.volume == sizeA + sizeB);
                REQUIRE(account.shares == 0);
                REQUIRE(account.capital == 0);
            }
        }
    }
}

//
// MARK: market
//

SCENARIO("a market order is submitted with no order in the book") {
    GIVEN("An empty limit order book") {
        Account account;
        LimitOrderBook book;
        auto size = 100;
        auto arrival = 1;
        WHEN("a buy market order is submitted") {
            book.market(&account, Side::Sell, size, arrival);
            THEN("the account should not be updated") {
                REQUIRE(account.order_count == 0);
                REQUIRE(account.volume == 0);
                REQUIRE(account.shares == 0);
                REQUIRE(account.capital == 0);
            }
        }
    }
}

SCENARIO("a market order is submitted with a perfect match") {
    GIVEN("An an order book with a limit order and a matched market order") {
        auto size = 100;
        auto price = 50;
        auto arrival1 = 1;
        auto arrival2 = 2;
        Account maker;
        Account taker;
        auto book = LimitOrderBook();

        WHEN("a sell market order is matched to a buy limit order") {
            book.limit(&maker, Side::Buy, size, price, arrival1);
            book.market(&taker, Side::Sell, size, arrival2);
            THEN("the limit_fill and market_fill functions should fire") {
                REQUIRE(0 == book.best_buy());
                REQUIRE(0 == book.volume(price));
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
                REQUIRE(taker.shares == -size);
                REQUIRE(taker.capital == price * size);
            }
        }
    }
}

SCENARIO("a market order is submitted that is partially filled by a limit") {
    GIVEN("An order book with a limit order and a smaller market order") {
        auto size_limit = 100;
        auto size_market = 20;
        auto price = 50;
        auto arrival1 = 1;
        auto arrival2 = 2;
        Account maker;
        Account taker;
        auto book = LimitOrderBook();
        book.limit(&maker, Side::Buy, size_limit, price, arrival1);

        WHEN("a buy market order is submitted") {
            book.market(&taker, Side::Sell, size_market, arrival2);
            THEN("the limit_fill and market_fill functions should fire") {
                REQUIRE(price == book.best_buy());
                REQUIRE(size_limit - size_market == book.volume(price));
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
                REQUIRE(taker.shares == -size_market);
                REQUIRE(taker.capital == price * size_market);
            }
        }
    }
}

SCENARIO("a market order is submitted that spans several limits") {
    GIVEN("An order book with two limits and a market order that requires both") {
        auto size_limit1 = 40;
        auto size_limit2 = 20;
        auto size_market = 50;
        auto price = 100;
        auto arrival1 = 1;
        auto arrival2 = 2;
        auto arrival3 = 3;
        Account maker1;
        Account maker2;
        Account taker;
        auto book = LimitOrderBook();
        book.limit(&maker1, Side::Buy, size_limit1, price, arrival1);
        book.limit(&maker2, Side::Buy, size_limit2, price, arrival2);

        WHEN("a buy market order is submitted") {
            book.market(&taker, Side::Sell, size_market, arrival3);
            THEN("the limit_fill and market_fill functions should fire") {
                REQUIRE(100 == book.best_buy());
                REQUIRE(size_limit1 + size_limit2 - size_market == book.volume(price));
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
                REQUIRE(taker.shares == -size_market);
                REQUIRE(taker.capital == price * size_market);
            }
        }
    }
}

SCENARIO("a market order is submitted that spans several limits and depletes book") {
    GIVEN("An order book with two limits and a market order that requires both") {
        auto size_limit1 = 20;
        auto size_limit2 = 20;
        auto size_market = 50;
        auto price = 100;
        auto arrival1 = 1;
        auto arrival2 = 2;
        auto arrival3 = 3;

        Account maker1;
        Account maker2;
        Account taker;
        auto book = LimitOrderBook();
        book.limit(&maker1, Side::Buy, size_limit1, price, arrival1);
        book.limit(&maker2, Side::Buy, size_limit2, price, arrival2);

        WHEN("a buy market order is submitted") {
            book.market(&taker, Side::Sell, size_market, arrival3);
            THEN("the limit_fill and market_fill functions should fire") {
                REQUIRE(0 == book.best_buy());
                REQUIRE(0 == book.volume(price));
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
                REQUIRE(taker.shares == -(size_limit1 + size_limit2));
                REQUIRE(taker.capital == price * (size_limit1 + size_limit2));
            }
        }
    }
}
