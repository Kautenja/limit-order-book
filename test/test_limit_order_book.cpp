// Test cases for an order book for managing Limit / Order objects in a continuous double auction.
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
#include "limit_order_book.hpp"

using namespace LOB;

// ---------------------------------------------------------------------------
// MARK: constructor
// ---------------------------------------------------------------------------

SCENARIO("initialize LimitOrderBook") {
    GIVEN("default parameters") {
        WHEN("a LimitOrderBook is initialized with no parameters") {
            REQUIRE_NOTHROW(new LimitOrderBook());
        }
    }
}

// ---------------------------------------------------------------------------
// MARK: limit
// ---------------------------------------------------------------------------

SCENARIO("send single order to LimitOrderBook") {
    GIVEN("an order book and a single sell order") {
        auto book = LimitOrderBook();
        auto side = Side::Sell;
        uint32_t size = 57;
        uint64_t price = 0xFEDCBA9876543210;
        uint64_t arrival = 0x1122334455667788;
        WHEN("the order is sent") {
            book.limit(side, 1, size, price, arrival);
            THEN("order ID is returned and the order is recorded") {
                REQUIRE(size == book.volume(price));
                REQUIRE(0 == book.volume(price - 1));
                REQUIRE(0 == book.volume(price + 1));
                REQUIRE(0 == book.best_buy());
                REQUIRE(price == book.best_sell());
            }
        }
    }
    GIVEN("an order book and a single buy order") {
        auto book = LimitOrderBook();
        auto side = Side::Buy;
        uint32_t size = 57;
        uint64_t price = 0xFEDCBA9876543210;
        uint64_t arrival = 0x1122334455667788;
        WHEN("the order is sent") {
            book.limit(side, 1, size, price, arrival);
            THEN("order ID is returned and the order is recorded") {
                REQUIRE(size == book.volume(price));
                REQUIRE(0 == book.volume(price - 1));
                REQUIRE(0 == book.volume(price + 1));
                REQUIRE(price == book.best_buy());
                REQUIRE(0 == book.best_sell());
            }
        }
    }
}

SCENARIO("send homogeneous orders to LimitOrderBook at same price") {
    GIVEN("an order book and a series of sell orders") {
        auto book = LimitOrderBook();
        auto side = Side::Sell;
        uint32_t sizeA = 50;
        uint32_t sizeB = 40;
        uint32_t sizeC = 30;
        uint64_t price = 0xFEDCBA9876543210;
        uint64_t arrivalA = 0;
        uint64_t arrivalB = 1;
        uint64_t arrivalC = 2;
        WHEN("the orders are sent") {
            book.limit(side, 1, sizeA, price, arrivalA);
            book.limit(side, 2, sizeB, price, arrivalB);
            book.limit(side, 3, sizeC, price, arrivalC);
            THEN("order ID is returned and the order is recorded") {
                REQUIRE(sizeA + sizeB + sizeC == book.volume(price));
                REQUIRE(0 == book.volume(price - 1));
                REQUIRE(0 == book.volume(price + 1));
                REQUIRE(0 == book.best_buy());
                REQUIRE(price == book.best_sell());
            }
        }
    }
    GIVEN("an order book and a series of buy orders") {
        auto book = LimitOrderBook();
        auto side = Side::Buy;
        uint32_t sizeA = 50;
        uint32_t sizeB = 40;
        uint32_t sizeC = 30;
        uint64_t price = 0xFEDCBA9876543210;
        uint64_t arrivalA = 0;
        uint64_t arrivalB = 1;
        uint64_t arrivalC = 2;
        WHEN("the orders are sent") {
            book.limit(side, 1, sizeA, price, arrivalA);
            book.limit(side, 2, sizeB, price, arrivalB);
            book.limit(side, 3, sizeC, price, arrivalC);
            THEN("order ID is returned and the order is recorded") {
                REQUIRE(sizeA + sizeB + sizeC == book.volume(price));
                REQUIRE(0 == book.volume(price - 1));
                REQUIRE(0 == book.volume(price + 1));
                REQUIRE(price == book.best_buy());
                REQUIRE(0 == book.best_sell());
            }
        }
    }
}

SCENARIO("send homogeneous orders to LimitOrderBook at different prices") {
    GIVEN("an order book and a series of sell orders") {
        auto book = LimitOrderBook();
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
            book.limit(side, 1, sizeA, priceA, arrivalA);
            book.limit(side, 2, sizeB, priceB, arrivalB);
            book.limit(side, 3, sizeC, priceC, arrivalC);
            THEN("order ID is returned and the order is recorded") {
                REQUIRE(sizeA == book.volume(priceA));
                REQUIRE(sizeB == book.volume(priceB));
                REQUIRE(sizeC == book.volume(priceC));
                REQUIRE(0 == book.best_buy());
                REQUIRE(priceB == book.best_sell());
            }
        }
    }
    GIVEN("an order book and a series of buy orders") {
        auto book = LimitOrderBook();
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
            book.limit(side, 1, sizeA, priceA, arrivalA);
            book.limit(side, 2, sizeB, priceB, arrivalB);
            book.limit(side, 3, sizeC, priceC, arrivalC);
            THEN("order ID is returned and the order is recorded") {
                REQUIRE(sizeA == book.volume(priceA));
                REQUIRE(sizeB == book.volume(priceB));
                REQUIRE(sizeC == book.volume(priceC));
                REQUIRE(priceB == book.best_buy());
                REQUIRE(0 == book.best_sell());
            }
        }
    }
}

SCENARIO("a limit order is submitted that crosses") {
    GIVEN("a book with 2 buy limit orders and a sell limit order") {
        auto book = LimitOrderBook();
        auto side = Side::Buy;
        uint32_t size = 20;
        uint32_t sizeMarket = 40;
        uint64_t priceA = 100;
        uint64_t priceB = 101;
        uint64_t arrivalA = 0;
        uint64_t arrivalB = 1;
        uint64_t arrivalC = 2;
        book.limit(side, 1, size, priceA, arrivalA);
        book.limit(side, 2, size, priceB, arrivalB);
        WHEN("the sell limit order is sent") {
            book.limit(!side, 3, sizeMarket, priceB, arrivalC);
            THEN("order ID is returned and the order is recorded") {
                // check the buy side
                REQUIRE(1 == book.size_buy());
                REQUIRE(size == book.volume_buy(priceA));
                REQUIRE(0 == book.volume_buy(priceB));
                REQUIRE(priceA == book.best_buy());
                // check the sell side
                REQUIRE(1 == book.size_sell());
                REQUIRE(sizeMarket - size == book.volume_sell(priceB));
                REQUIRE(priceB == book.best_sell());
            }
        }
    }
    GIVEN("a book with 2 sell limit orders and a buy limit order") {
        auto book = LimitOrderBook();
        auto side = Side::Sell;
        uint32_t size = 20;
        uint32_t sizeMarket = 40;
        uint64_t priceA = 101;
        uint64_t priceB = 100;
        uint64_t arrivalA = 0;
        uint64_t arrivalB = 1;
        uint64_t arrivalC = 2;
        book.limit(side, 1, size, priceA, arrivalA);
        book.limit(side, 2, size, priceB, arrivalB);
        WHEN("the sell limit order is sent") {
            book.limit(!side, 3, sizeMarket, priceB, arrivalC);
            THEN("order ID is returned and the order is recorded") {
                // check the sell side
                REQUIRE(1 == book.size_sell());
                REQUIRE(size == book.volume_sell(priceA));
                REQUIRE(0 == book.volume_sell(priceB));
                REQUIRE(priceA == book.best_sell());
                // check the buy side
                REQUIRE(1 == book.size_buy());
                REQUIRE(sizeMarket - size == book.volume_buy(priceB));
                REQUIRE(priceB == book.best_buy());

            }
        }
    }
}

SCENARIO("a limit order is submitted that crosses and fills") {
    GIVEN("a book with a buy limit order") {
        auto book = LimitOrderBook();
        auto side = Side::Buy;
        uint32_t size = 20;
        uint64_t price = 100;
        uint64_t arrivalA = 0;
        uint64_t arrivalB = 1;
        book.limit(side, 1, size, price, arrivalA);
        WHEN("the sell limit order is sent") {
            book.limit(!side, 2, size, price, arrivalB);
            THEN("order ID is returned and the order is recorded") {
                // check the buy side
                REQUIRE(0 == book.size_buy());
                REQUIRE(0 == book.volume_buy(price));
                REQUIRE(0 == book.volume_buy());
                REQUIRE(0 == book.best_buy());
                // check the sell side
                REQUIRE(0 == book.size_sell());
                REQUIRE(0 == book.volume_sell(price));
                REQUIRE(0 == book.best_sell());
            }
        }
    }
    GIVEN("a book with a sell limit order") {
        auto book = LimitOrderBook();
        auto side = Side::Sell;
        uint32_t size = 20;
        uint64_t price = 100;
        uint64_t arrivalA = 0;
        uint64_t arrivalB = 1;
        book.limit(side, 1, size, price, arrivalA);
        WHEN("the sell limit order is sent") {
            book.limit(!side, 2, size, price, arrivalB);
            THEN("order ID is returned and the order is recorded") {
                // check the sell side
                REQUIRE(0 == book.size_sell());
                REQUIRE(0 == book.volume_sell(price));
                REQUIRE(0 == book.volume_sell());
                REQUIRE(0 == book.best_sell());
                // check the buy side
                REQUIRE(0 == book.size_buy());
                REQUIRE(0 == book.volume_buy(price));
                REQUIRE(0 == book.best_buy());
            }
        }
    }
}

// ---------------------------------------------------------------------------
// MARK: cancel
// ---------------------------------------------------------------------------

// limit removal

SCENARIO(R"(LOB
tree shape (single node):
*
)") {
    GIVEN("an order book and an ID for a single sell order") {
        auto book = LimitOrderBook();
        auto side = Side::Sell;
        uint32_t size = 50;
        uint64_t price = 3253;
        uint64_t arrival = 0x1122334455667788;
        book.limit(side, 1, size, price, arrival);
        WHEN("the order is canceled") {
            book.cancel(1);
            THEN("the limit is cleared") {
                REQUIRE(0 == book.volume(price));
                REQUIRE(0 == book.best_buy());
                REQUIRE(0 == book.best_sell());
            }
        }
        // tree integrity check
        WHEN("the order is duplicated, added, and canceled again") {
            book.cancel(1);
            book.limit(side, 2, size, price, arrival + 1);
            book.cancel(2);
            THEN("the limit is cleared") {
                REQUIRE(0 == book.volume(price));
                REQUIRE(0 == book.best_buy());
                REQUIRE(0 == book.best_sell());
            }
        }
    }
    GIVEN("an order book and an ID for a single buy order") {
        auto book = LimitOrderBook();
        auto side = Side::Buy;
        uint32_t size = 50;
        uint64_t price = 3253;
        uint64_t arrival = 0x1122334455667788;
        book.limit(side, 1, size, price, arrival);
        WHEN("the orders is canceled") {
            book.cancel(1);
            THEN("the limit is cleared") {
                REQUIRE(0 == book.volume(price));
                REQUIRE(0 == book.best_buy());
                REQUIRE(0 == book.best_sell());
            }
        }
        // tree integrity check
        WHEN("the order is duplicated, added, and canceled again") {
            book.cancel(1);
            book.limit(side, 2, size, price, arrival + 1);
            book.cancel(2);
            THEN("the limit is cleared") {
                REQUIRE(0 == book.volume(price));
                REQUIRE(0 == book.best_buy());
                REQUIRE(0 == book.best_sell());
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
        auto side = Side::Sell;
        // submit the MIDDLE order first (price-wise)
        book.limit(side, 1, size, prices[1], arrivals[0]);
        // the lowest price will be the left child
        book.limit(side, 2, size, prices[0], arrivals[1]);
        // the highest price will be the right child
        book.limit(side, 3, size, prices[2], arrivals[2]);
        WHEN("the left order is canceled") {
            book.cancel(2);
            THEN("the limit is cleared") {
                REQUIRE(0 == book.volume(prices[0]));
                REQUIRE(size == book.volume(prices[1]));
                REQUIRE(size == book.volume(prices[2]));
                REQUIRE(prices[1] == book.best_sell());
            }
        }
        // tree integrity check
        WHEN("the left order is duplicated, added, and canceled again") {
            book.cancel(2);
            book.limit(side, 4, size, prices[0], arrivals[3]);
            book.cancel(4);
            THEN("the limit is cleared") {
                REQUIRE(0 == book.volume(prices[0]));
                REQUIRE(size == book.volume(prices[1]));
                REQUIRE(size == book.volume(prices[2]));
                REQUIRE(prices[1] == book.best_sell());
            }
        }
    }
    GIVEN("an order book with V shaped limit tree (buy)") {
        auto book = LimitOrderBook();
        auto side = Side::Buy;
        // submit the MIDDLE order first (price-wise)
        book.limit(side, 1, size, prices[1], arrivals[0]);
        // the lowest price will be the left child
        book.limit(side, 2, size, prices[0], arrivals[1]);
        // the highest price will be the right child
        book.limit(side, 3, size, prices[2], arrivals[2]);
        WHEN("the left order is canceled") {
            book.cancel(2);
            THEN("the limit is cleared") {
                REQUIRE(0 == book.volume(prices[0]));
                REQUIRE(size == book.volume(prices[1]));
                REQUIRE(size == book.volume(prices[2]));
                REQUIRE(prices[2] == book.best_buy());
            }
        }
        // tree integrity check
        WHEN("the left order is duplicated, added, and canceled again") {
            book.cancel(2);
            book.limit(side, 4, size, prices[0], arrivals[3]);
            book.cancel(4);
            THEN("the limit is cleared") {
                REQUIRE(0 == book.volume(prices[0]));
                REQUIRE(size == book.volume(prices[1]));
                REQUIRE(size == book.volume(prices[2]));
                REQUIRE(prices[2] == book.best_buy());
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
        auto side = Side::Sell;
        // submit the MIDDLE order first (price-wise)
        book.limit(side, 1, size, prices[1], arrivals[0]);
        // the lowest price will be the left child
        book.limit(side, 2, size, prices[0], arrivals[1]);
        // the highest price will be the right child
        book.limit(side, 3, size, prices[2], arrivals[2]);
        WHEN("the right order is canceled") {
            book.cancel(3);
            THEN("the limit is cleared") {
                REQUIRE(size == book.volume(prices[0]));
                REQUIRE(size == book.volume(prices[1]));
                REQUIRE(0 == book.volume(prices[2]));
                REQUIRE(prices[0] == book.best_sell());
            }
        }
        // tree integrity check
        WHEN("the right order is duplicated, added, and canceled again") {
            book.cancel(3);
            book.limit(side, 4, size, prices[2], arrivals[3]);
            book.cancel(4);
            THEN("the limit is cleared") {
                REQUIRE(size == book.volume(prices[0]));
                REQUIRE(size == book.volume(prices[1]));
                REQUIRE(0 == book.volume(prices[2]));
                REQUIRE(prices[0] == book.best_sell());
            }
        }
    }
    GIVEN("an order book with V shaped limit tree (buy)") {
        auto book = LimitOrderBook();
        auto side = Side::Buy;
        // submit the MIDDLE order first (price-wise)
        book.limit(side, 1, size, prices[1], arrivals[0]);
        // the lowest price will be the left child
        book.limit(side, 2, size, prices[0], arrivals[1]);
        // the highest price will be the right child
        book.limit(side, 3, size, prices[2], arrivals[2]);
        WHEN("the right order is canceled") {
            book.cancel(3);
            THEN("the limit is cleared") {
                REQUIRE(size == book.volume(prices[0]));
                REQUIRE(size == book.volume(prices[1]));
                REQUIRE(0 == book.volume(prices[2]));
                REQUIRE(prices[1] == book.best_buy());
            }
        }
        // tree integrity check
        WHEN("the right order is duplicated, added, and canceled again") {
            book.cancel(3);
            book.limit(side, 4, size, prices[2], arrivals[3]);
            book.cancel(4);
            THEN("the limit is cleared") {
                REQUIRE(size == book.volume(prices[0]));
                REQUIRE(size == book.volume(prices[1]));
                REQUIRE(0 == book.volume(prices[2]));
                REQUIRE(prices[1] == book.best_buy());
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
        auto side = Side::Sell;
        // submit the MIDDLE order first (price-wise)
        book.limit(side, 1, size, prices[1], arrivals[0]);
        // the lowest price will be the left child
        book.limit(side, 2, size, prices[0], arrivals[1]);
        // the highest price will be the right child
        book.limit(side, 3, size, prices[2], arrivals[2]);
        WHEN("the root order is canceled") {
            book.cancel(1);
            THEN("the limit is cleared") {
                REQUIRE(size == book.volume(prices[0]));
                REQUIRE(0 == book.volume(prices[1]));
                REQUIRE(size == book.volume(prices[2]));
                REQUIRE(prices[0] == book.best_sell());
            }
        }
        // tree integrity check
        WHEN("the root order is duplicated, added, and canceled again") {
            book.cancel(1);
            book.limit(side, 4, size, prices[1], arrivals[3]);
            book.cancel(4);
            THEN("the limit is cleared") {
                REQUIRE(size == book.volume(prices[0]));
                REQUIRE(0 == book.volume(prices[1]));
                REQUIRE(size == book.volume(prices[2]));
                REQUIRE(prices[0] == book.best_sell());
            }
        }
    }
    GIVEN("an order book with V shaped limit tree (buy)") {
        auto book = LimitOrderBook();
        auto side = Side::Buy;
        // submit the MIDDLE order first (price-wise)
        book.limit(side, 1, size, prices[1], arrivals[0]);
        // the lowest price will be the left child
        book.limit(side, 2, size, prices[0], arrivals[1]);
        // the highest price will be the right child
        book.limit(side, 3, size, prices[2], arrivals[2]);
        WHEN("the root order is canceled") {
            book.cancel(1);
            THEN("the limit is cleared") {
                REQUIRE(size == book.volume(prices[0]));
                REQUIRE(0 == book.volume(prices[1]));
                REQUIRE(size == book.volume(prices[2]));
                REQUIRE(prices[2] == book.best_buy());
            }
        }
        // tree integrity check
        WHEN("the root order is duplicated, added, and canceled again") {
            book.cancel(1);
            book.limit(side, 4, size, prices[1], arrivals[3]);
            book.cancel(4);
            THEN("the limit is cleared") {
                REQUIRE(size == book.volume(prices[0]));
                REQUIRE(0 == book.volume(prices[1]));
                REQUIRE(size == book.volume(prices[2]));
                REQUIRE(prices[2] == book.best_buy());
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
        auto side = Side::Sell;
        book.limit(side, 1, size, prices[0], arrivals[0]);
        book.limit(side, 2, size, prices[1], arrivals[1]);
        book.limit(side, 3, size, prices[2], arrivals[2]);
        WHEN("the root order is canceled") {
            book.cancel(1);
            THEN("the limit is cleared") {
                REQUIRE(0 == book.volume(prices[0]));
                REQUIRE(size == book.volume(prices[1]));
                REQUIRE(size == book.volume(prices[2]));
                REQUIRE(prices[1] == book.best_sell());
            }
        }
        // tree integrity check
        WHEN("the root order is duplicated, added, and canceled again") {
            book.cancel(1);
            book.limit(side, 4, size, prices[0], arrivals[3]);
            book.cancel(4);
            THEN("the limit is cleared") {
                REQUIRE(0 == book.volume(prices[0]));
                REQUIRE(size == book.volume(prices[1]));
                REQUIRE(size == book.volume(prices[2]));
                REQUIRE(prices[1] == book.best_sell());
            }
        }
    }
    GIVEN("an order book with right leg shaped limit tree (buy)") {
        auto book = LimitOrderBook();
        auto side = Side::Buy;
        book.limit(side, 1, size, prices[0], arrivals[0]);
        book.limit(side, 2, size, prices[1], arrivals[1]);
        book.limit(side, 3, size, prices[2], arrivals[2]);
        WHEN("the root order is canceled") {
            book.cancel(1);
            THEN("the limit is cleared") {
                REQUIRE(0 == book.volume(prices[0]));
                REQUIRE(size == book.volume(prices[1]));
                REQUIRE(size == book.volume(prices[2]));
                REQUIRE(prices[2] == book.best_buy());
            }
        }
        // tree integrity check
        WHEN("the root order is duplicated, added, and canceled again") {
            book.cancel(1);
            book.limit(side, 4, size, prices[0], arrivals[3]);
            book.cancel(4);
            THEN("the limit is cleared") {
                REQUIRE(0 == book.volume(prices[0]));
                REQUIRE(size == book.volume(prices[1]));
                REQUIRE(size == book.volume(prices[2]));
                REQUIRE(prices[2] == book.best_buy());
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
        auto side = Side::Sell;
        book.limit(side, 1, size, prices[0], arrivals[0]);
        book.limit(side, 2, size, prices[1], arrivals[1]);
        book.limit(side, 3, size, prices[2], arrivals[2]);
        WHEN("the middle order is canceled") {
            book.cancel(2);
            THEN("the limit is cleared") {
                REQUIRE(size == book.volume(prices[0]));
                REQUIRE(0 == book.volume(prices[1]));
                REQUIRE(size == book.volume(prices[2]));
                REQUIRE(prices[0] == book.best_sell());
            }
        }
        // tree integrity check
        WHEN("the middle order is duplicated, added, and canceled again") {
            book.cancel(2);
            book.limit(side, 4, size, prices[1], arrivals[3]);
            book.cancel(4);
            THEN("the limit is cleared") {
                REQUIRE(size == book.volume(prices[0]));
                REQUIRE(0 == book.volume(prices[1]));
                REQUIRE(size == book.volume(prices[2]));
                REQUIRE(prices[0] == book.best_sell());
            }
        }
    }
    GIVEN("an order book with right leg shaped limit tree (buy)") {
        auto book = LimitOrderBook();
        auto side = Side::Buy;
        book.limit(side, 1, size, prices[0], arrivals[0]);
        book.limit(side, 2, size, prices[1], arrivals[1]);
        book.limit(side, 3, size, prices[2], arrivals[2]);
        WHEN("the middle order is canceled") {
            book.cancel(2);
            THEN("the limit is cleared") {
                REQUIRE(size == book.volume(prices[0]));
                REQUIRE(0 == book.volume(prices[1]));
                REQUIRE(size == book.volume(prices[2]));
                REQUIRE(prices[2] == book.best_buy());
            }
        }
        // tree integrity check
        WHEN("the middle order is duplicated, added, and canceled again") {
            book.cancel(2);
            book.limit(side, 4, size, prices[1], arrivals[3]);
            book.cancel(4);
            THEN("the limit is cleared") {
                REQUIRE(size == book.volume(prices[0]));
                REQUIRE(0 == book.volume(prices[1]));
                REQUIRE(size == book.volume(prices[2]));
                REQUIRE(prices[2] == book.best_buy());
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
        auto side = Side::Sell;
        book.limit(side, 1, size, prices[0], arrivals[0]);
        book.limit(side, 2, size, prices[1], arrivals[1]);
        book.limit(side, 3, size, prices[2], arrivals[2]);
        WHEN("the leaf order is canceled") {
            book.cancel(3);
            THEN("the limit is cleared") {
                REQUIRE(size == book.volume(prices[0]));
                REQUIRE(size == book.volume(prices[1]));
                REQUIRE(0 == book.volume(prices[2]));
                REQUIRE(prices[0] == book.best_sell());
            }
        }
        // tree integrity check
        WHEN("the leaf order is duplicated, added, and canceled again") {
            book.cancel(3);
            book.limit(side, 4, size, prices[2], arrivals[3]);
            book.cancel(4);
            THEN("the limit is cleared") {
                REQUIRE(size == book.volume(prices[0]));
                REQUIRE(size == book.volume(prices[1]));
                REQUIRE(0 == book.volume(prices[2]));
                REQUIRE(prices[0] == book.best_sell());
            }
        }
    }
    GIVEN("an order book with right leg shaped limit tree (buy)") {
        auto book = LimitOrderBook();
        auto side = Side::Buy;
        book.limit(side, 1, size, prices[0], arrivals[0]);
        book.limit(side, 2, size, prices[1], arrivals[1]);
        book.limit(side, 3, size, prices[2], arrivals[2]);
        WHEN("the leaf order is canceled") {
            book.cancel(3);
            THEN("the limit is cleared") {
                REQUIRE(size == book.volume(prices[0]));
                REQUIRE(size == book.volume(prices[1]));
                REQUIRE(0 == book.volume(prices[2]));
                REQUIRE(prices[1] == book.best_buy());
            }
        }
        // tree integrity check
        WHEN("the leaf order is duplicated, added, and canceled again") {
            book.cancel(3);
            book.limit(side, 4, size, prices[2], arrivals[3]);
            book.cancel(4);
            THEN("the limit is cleared") {
                REQUIRE(size == book.volume(prices[0]));
                REQUIRE(size == book.volume(prices[1]));
                REQUIRE(0 == book.volume(prices[2]));
                REQUIRE(prices[1] == book.best_buy());
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
        auto side = Side::Sell;
        book.limit(side, 1, size, prices[2], arrivals[0]);
        book.limit(side, 2, size, prices[1], arrivals[1]);
        book.limit(side, 3, size, prices[0], arrivals[2]);
        WHEN("the root order is canceled") {
            book.cancel(1);
            THEN("the limit is cleared") {
                REQUIRE(size == book.volume(prices[0]));
                REQUIRE(size == book.volume(prices[1]));
                REQUIRE(0 == book.volume(prices[2]));
                REQUIRE(prices[0] == book.best_sell());
            }
        }
        // tree integrity check
        WHEN("the root order is duplicated, added, and canceled again") {
            book.cancel(1);
            book.limit(side, 4, size, prices[2], arrivals[3]);
            book.cancel(4);
            THEN("the limit is cleared") {
                REQUIRE(size == book.volume(prices[0]));
                REQUIRE(size == book.volume(prices[1]));
                REQUIRE(0 == book.volume(prices[2]));
                REQUIRE(prices[0] == book.best_sell());
            }
        }
    }
    GIVEN("an order book with left leg shaped limit tree (buy)") {
        auto book = LimitOrderBook();
        auto side = Side::Buy;
        book.limit(side, 1, size, prices[2], arrivals[0]);
        book.limit(side, 2, size, prices[1], arrivals[1]);
        book.limit(side, 3, size, prices[0], arrivals[2]);
        WHEN("the root order is canceled") {
            book.cancel(1);
            THEN("the limit is cleared") {
                REQUIRE(size == book.volume(prices[0]));
                REQUIRE(size == book.volume(prices[1]));
                REQUIRE(0 == book.volume(prices[2]));
                REQUIRE(prices[1] == book.best_buy());
            }
        }
        // tree integrity check
        WHEN("the root order is duplicated, added, and canceled again") {
            book.cancel(1);
            book.limit(side, 4, size, prices[2], arrivals[3]);
            book.cancel(4);
            THEN("the limit is cleared") {
                REQUIRE(size == book.volume(prices[0]));
                REQUIRE(size == book.volume(prices[1]));
                REQUIRE(0 == book.volume(prices[2]));
                REQUIRE(prices[1] == book.best_buy());
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
        auto side = Side::Sell;
        book.limit(side, 1, size, prices[2], arrivals[0]);
        book.limit(side, 2, size, prices[1], arrivals[1]);
        book.limit(side, 3, size, prices[0], arrivals[2]);
        WHEN("the middle order is canceled") {
            book.cancel(2);
            THEN("the limit is cleared") {
                REQUIRE(size == book.volume(prices[0]));
                REQUIRE(0 == book.volume(prices[1]));
                REQUIRE(size == book.volume(prices[2]));
                REQUIRE(prices[0] == book.best_sell());
            }
        }
        // tree integrity check
        WHEN("the middle order is duplicated, added, and canceled again") {
            book.cancel(2);
            book.limit(side, 4, size, prices[1], arrivals[3]);
            book.cancel(4);
            THEN("the limit is cleared") {
                REQUIRE(size == book.volume(prices[0]));
                REQUIRE(0 == book.volume(prices[1]));
                REQUIRE(size == book.volume(prices[2]));
                REQUIRE(prices[0] == book.best_sell());
            }
        }
    }
    GIVEN("an order book with left leg shaped limit tree (buy)") {
        auto book = LimitOrderBook();
        auto side = Side::Buy;
        book.limit(side, 1, size, prices[2], arrivals[0]);
        book.limit(side, 2, size, prices[1], arrivals[1]);
        book.limit(side, 3, size, prices[0], arrivals[2]);
        WHEN("the middle order is canceled") {
            book.cancel(2);
            THEN("the limit is cleared") {
                REQUIRE(size == book.volume(prices[0]));
                REQUIRE(0 == book.volume(prices[1]));
                REQUIRE(size == book.volume(prices[2]));
                REQUIRE(prices[2] == book.best_buy());
            }
        }
        // tree integrity check
        WHEN("the middle order is duplicated, added, and canceled again") {
            book.cancel(2);
            book.limit(side, 4, size, prices[1], arrivals[3]);
            book.cancel(4);
            THEN("the limit is cleared") {
                REQUIRE(size == book.volume(prices[0]));
                REQUIRE(0 == book.volume(prices[1]));
                REQUIRE(size == book.volume(prices[2]));
                REQUIRE(prices[2] == book.best_buy());
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
        auto side = Side::Sell;
        book.limit(side, 1, size, prices[2], arrivals[0]);
        book.limit(side, 2, size, prices[1], arrivals[1]);
        book.limit(side, 3, size, prices[0], arrivals[2]);
        WHEN("the leaf order is canceled") {
            book.cancel(3);
            THEN("the limit is cleared") {
                REQUIRE(0 == book.volume(prices[0]));
                REQUIRE(size == book.volume(prices[1]));
                REQUIRE(size == book.volume(prices[2]));
                REQUIRE(prices[1] == book.best_sell());
            }
        }
        // tree integrity check
        WHEN("the leaf order is duplicated, added, and canceled again") {
            book.cancel(3);
            book.limit(side, 4, size, prices[0], arrivals[3]);
            book.cancel(4);
            THEN("the limit is cleared") {
                REQUIRE(0 == book.volume(prices[0]));
                REQUIRE(size == book.volume(prices[1]));
                REQUIRE(size == book.volume(prices[2]));
                REQUIRE(prices[1] == book.best_sell());
            }
        }
    }
    GIVEN("an order book with left leg shaped limit tree (buy)") {
        auto book = LimitOrderBook();
        auto side = Side::Buy;
        book.limit(side, 1, size, prices[2], arrivals[0]);
        book.limit(side, 2, size, prices[1], arrivals[1]);
        book.limit(side, 3, size, prices[0], arrivals[2]);
        WHEN("the leaf order is canceled") {
            book.cancel(3);
            THEN("the limit is cleared") {
                REQUIRE(0 == book.volume(prices[0]));
                REQUIRE(size == book.volume(prices[1]));
                REQUIRE(size == book.volume(prices[2]));
                REQUIRE(prices[2] == book.best_buy());
            }
        }
        // tree integrity check
        WHEN("the leaf order is duplicated, added, and canceled again") {
            book.cancel(3);
            book.limit(side, 4, size, prices[0], arrivals[3]);
            book.cancel(4);
            THEN("the limit is cleared") {
                REQUIRE(0 == book.volume(prices[0]));
                REQUIRE(size == book.volume(prices[1]));
                REQUIRE(size == book.volume(prices[2]));
                REQUIRE(prices[2] == book.best_buy());
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
        auto side = Side::Buy;
        // submit the MIDDLE order first (price-wise)
        book.limit(side, 1, size, prices[1], arrivals[0]);
        // the lowest price will be the left child
        book.limit(side, 2, size, prices[0], arrivals[1]);
        // the highest price will be the right child
        book.limit(side, 3, size, prices[2], arrivals[2]);
        // the last price will be the left branch of the right child
        book.limit(side, 4, size, prices[3], arrivals[3]);
        WHEN("the root order is canceled") {
            book.cancel(1);
            THEN("the limit is cleared") {
                REQUIRE(size == book.volume(prices[0]));
                REQUIRE(0 == book.volume(prices[1]));
                REQUIRE(size == book.volume(prices[2]));
                REQUIRE(size == book.volume(prices[3]));
                REQUIRE(prices[2] == book.best_buy());
            }
        }
        // tree integrity check
        WHEN("the root order is duplicated, added, and canceled again") {
            book.cancel(1);
            book.limit(side, 5, size, prices[1], arrivals[4]);
            book.cancel(5);
            THEN("the limit is cleared") {
                REQUIRE(size == book.volume(prices[0]));
                REQUIRE(0 == book.volume(prices[1]));
                REQUIRE(size == book.volume(prices[2]));
                REQUIRE(size == book.volume(prices[3]));
                REQUIRE(prices[2] == book.best_buy());
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
        auto side = Side::Buy;
        // submit the MIDDLE order first (price-wise)
        book.limit(side, 1, size, prices[1], arrivals[0]);
        // the lowest price will be the left child
        book.limit(side, 2, size, prices[0], arrivals[1]);
        // the highest price will be the right child
        book.limit(side, 3, size, prices[2], arrivals[2]);
        // the last price will be the left branch of the right child
        book.limit(side, 4, size, prices[3], arrivals[3]);
        // the last price will be the left branch of the right child
        book.limit(side, 5, size, prices[4], arrivals[4]);
        WHEN("the root order is canceled") {
            book.cancel(1);
            THEN("the limit is cleared") {
                REQUIRE(size == book.volume(prices[0]));
                REQUIRE(0 == book.volume(prices[1]));
                REQUIRE(size == book.volume(prices[2]));
                REQUIRE(size == book.volume(prices[3]));
                REQUIRE(size == book.volume(prices[4]));
                REQUIRE(prices[2] == book.best_buy());
            }
        }
        // tree integrity check
        WHEN("the root order is duplicated, added, and canceled again") {
            book.cancel(1);
            book.limit(side, 6, size, prices[1], arrivals[5]);
            book.cancel(6);
            THEN("the limit is cleared") {
                REQUIRE(size == book.volume(prices[0]));
                REQUIRE(0 == book.volume(prices[1]));
                REQUIRE(size == book.volume(prices[2]));
                REQUIRE(size == book.volume(prices[3]));
                REQUIRE(size == book.volume(prices[4]));
                REQUIRE(prices[2] == book.best_buy());
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
        auto side = Side::Buy;
        book.limit(side, 1, size, prices[0], arrivals[0]);
        book.limit(side, 2, size, prices[1], arrivals[1]);
        book.limit(side, 3, size, prices[2], arrivals[2]);
        book.limit(side, 4, size, prices[3], arrivals[3]);
        WHEN("the root-child order is canceled") {
            book.cancel(2);
            THEN("the limit is cleared") {
                REQUIRE(size == book.volume(prices[0]));
                REQUIRE(0 == book.volume(prices[1]));
                REQUIRE(size == book.volume(prices[2]));
                REQUIRE(size == book.volume(prices[3]));
                REQUIRE(prices[3] == book.best_buy());
            }
        }
        // tree integrity check
        WHEN("the root-child order is duplicated, added, and canceled again") {
            book.cancel(2);
            book.limit(side, 5, size, prices[1], arrivals[4]);
            book.cancel(5);
            THEN("the limit is cleared") {
                REQUIRE(size == book.volume(prices[0]));
                REQUIRE(0 == book.volume(prices[1]));
                REQUIRE(size == book.volume(prices[2]));
                REQUIRE(size == book.volume(prices[3]));
                REQUIRE(prices[3] == book.best_buy());
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
        auto side = Side::Sell;
        book.limit(side, 1, size, prices[0], arrivals[0]);
        book.limit(side, 2, size, prices[1], arrivals[1]);
        book.limit(side, 3, size, prices[2], arrivals[2]);
        book.limit(side, 4, size, prices[3], arrivals[3]);
        WHEN("the root-child order is canceled") {
            book.cancel(2);
            THEN("the limit is cleared") {
                REQUIRE(size == book.volume(prices[0]));
                REQUIRE(0 == book.volume(prices[1]));
                REQUIRE(size == book.volume(prices[2]));
                REQUIRE(size == book.volume(prices[3]));
                REQUIRE(prices[3] == book.best_sell());
            }
        }
        // tree integrity check
        WHEN("the root-child order is duplicated, added, and canceled again") {
            book.cancel(2);
            book.limit(side, 5, size, prices[1], arrivals[4]);
            book.cancel(5);
            THEN("the limit is cleared") {
                REQUIRE(size == book.volume(prices[0]));
                REQUIRE(0 == book.volume(prices[1]));
                REQUIRE(size == book.volume(prices[2]));
                REQUIRE(size == book.volume(prices[3]));
                REQUIRE(prices[3] == book.best_sell());
            }
        }
    }
}

// order removal

SCENARIO("cancel first order in a Limit queue of orders") {
    GIVEN("an order book and a Limit queue of sell orders") {
        auto book = LimitOrderBook();
        auto side = Side::Sell;
        uint32_t sizeA = 50;
        uint32_t sizeB = 40;
        uint32_t sizeC = 30;
        uint64_t price = 3253;
        uint64_t arrivalA = 0;
        uint64_t arrivalB = 1;
        uint64_t arrivalC = 2;
        book.limit(side, 1, sizeA, price, arrivalA);
        book.limit(side, 2, sizeB, price, arrivalB);
        book.limit(side, 3, sizeC, price, arrivalC);
        WHEN("the first order is canceled") {
            book.cancel(1);
            THEN("order ID is returned and the order is recorded") {
                REQUIRE(sizeB + sizeC == book.volume(price));
                REQUIRE(0 == book.best_buy());
                REQUIRE(price == book.best_sell());
            }
        }
    }
    GIVEN("an order book and a Limit queue of buy orders") {
        auto book = LimitOrderBook();
        auto side = Side::Buy;
        uint32_t sizeA = 50;
        uint32_t sizeB = 40;
        uint32_t sizeC = 30;
        uint64_t price = 3253;
        uint64_t arrivalA = 0;
        uint64_t arrivalB = 1;
        uint64_t arrivalC = 2;
        book.limit(side, 1, sizeA, price, arrivalA);
        book.limit(side, 2, sizeB, price, arrivalB);
        book.limit(side, 3, sizeC, price, arrivalC);
        WHEN("the first order is canceled") {
            book.cancel(1);
            THEN("order ID is returned and the order is recorded") {
                REQUIRE(sizeB + sizeC == book.volume(price));
                REQUIRE(price == book.best_buy());
                REQUIRE(0 == book.best_sell());
            }
        }
    }
}

SCENARIO("cancel middle order in a Limit queue of orders") {
    GIVEN("an order book and a Limit queue of sell orders") {
        auto book = LimitOrderBook();
        auto side = Side::Sell;
        uint32_t sizeA = 50;
        uint32_t sizeB = 40;
        uint32_t sizeC = 30;
        uint64_t price = 3253;
        uint64_t arrivalA = 0;
        uint64_t arrivalB = 1;
        uint64_t arrivalC = 2;
        book.limit(side, 1, sizeA, price, arrivalA);
        book.limit(side, 2, sizeB, price, arrivalB);
        book.limit(side, 3, sizeC, price, arrivalC);
        WHEN("the middle order is canceled") {
            book.cancel(2);
            THEN("order ID is returned and the order is recorded") {
                REQUIRE(sizeA + sizeC == book.volume(price));
                REQUIRE(0 == book.best_buy());
                REQUIRE(price == book.best_sell());
            }
        }
    }
    GIVEN("an order book and a Limit queue of buy orders") {
        auto book = LimitOrderBook();
        auto side = Side::Buy;
        uint32_t sizeA = 50;
        uint32_t sizeB = 40;
        uint32_t sizeC = 30;
        uint64_t price = 3253;
        uint64_t arrivalA = 0;
        uint64_t arrivalB = 1;
        uint64_t arrivalC = 2;
        book.limit(side, 1, sizeA, price, arrivalA);
        book.limit(side, 2, sizeB, price, arrivalB);
        book.limit(side, 3, sizeC, price, arrivalC);
        WHEN("the middle order is canceled") {
            book.cancel(2);
            THEN("order ID is returned and the order is recorded") {
                REQUIRE(sizeA + sizeC == book.volume(price));
                REQUIRE(price == book.best_buy());
                REQUIRE(0 == book.best_sell());
            }
        }
    }
}

SCENARIO("cancel last order in a Limit queue of orders") {
    GIVEN("an order book and a Limit queue of sell orders") {
        auto book = LimitOrderBook();
        auto side = Side::Sell;
        uint32_t sizeA = 50;
        uint32_t sizeB = 40;
        uint32_t sizeC = 30;
        uint64_t price = 3253;
        uint64_t arrivalA = 0;
        uint64_t arrivalB = 1;
        uint64_t arrivalC = 2;
        book.limit(side, 1, sizeA, price, arrivalA);
        book.limit(side, 2, sizeB, price, arrivalB);
        book.limit(side, 3, sizeC, price, arrivalC);
        WHEN("the last order is canceled") {
            book.cancel(3);
            THEN("order ID is returned and the order is recorded") {
                REQUIRE(sizeA + sizeB == book.volume(price));
                REQUIRE(0 == book.best_buy());
                REQUIRE(price == book.best_sell());
            }
        }
    }
    GIVEN("an order book and a Limit queue of buy orders") {
        auto book = LimitOrderBook();
        auto side = Side::Buy;
        uint32_t sizeA = 50;
        uint32_t sizeB = 40;
        uint32_t sizeC = 30;
        uint64_t price = 3253;
        uint64_t arrivalA = 0;
        uint64_t arrivalB = 1;
        uint64_t arrivalC = 2;
        book.limit(side, 1, sizeA, price, arrivalA);
        book.limit(side, 2, sizeB, price, arrivalB);
        book.limit(side, 3, sizeC, price, arrivalC);
        WHEN("the last order is canceled") {
            book.cancel(3);
            THEN("order ID is returned and the order is recorded") {
                REQUIRE(sizeA + sizeB == book.volume(price));
                REQUIRE(price == book.best_buy());
                REQUIRE(0 == book.best_sell());
            }
        }
    }
}

// ---------------------------------------------------------------------------
// MARK: market
// ---------------------------------------------------------------------------

SCENARIO("a market order is submitted with no order in the book") {
    GIVEN("An empty limit order book") {
        LimitOrderBook book;
        auto size = 100;
        auto arrival = 1;
        WHEN("a buy market order is submitted") {
            book.market(Side::Sell, 1, size, arrival);
        }
    }
}

SCENARIO("a market order is submitted with a perfect match") {
    GIVEN("An an order book with a limit order and a matched market order") {
        auto size = 100;
        auto price = 50;
        auto arrival1 = 1;
        auto arrival2 = 2;
        auto book = LimitOrderBook();

        WHEN("a sell market order is matched to a buy limit order") {
            book.limit(Side::Buy, 1, size, price, arrival1);
            book.market(Side::Sell, 2, size, arrival2);
            THEN("the limit_fill and market_fill functions should fire") {
                REQUIRE(0 == book.best_buy());
                REQUIRE(0 == book.volume(price));
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
        auto book = LimitOrderBook();
        book.limit(Side::Buy, 1, size_limit, price, arrival1);

        WHEN("a buy market order is submitted") {
            book.market(Side::Sell, 2, size_market, arrival2);
            THEN("the limit_fill and market_fill functions should fire") {
                REQUIRE(price == book.best_buy());
                REQUIRE(size_limit - size_market == book.volume(price));
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
        auto book = LimitOrderBook();
        book.limit(Side::Buy, 1, size_limit1, price, arrival1);
        book.limit(Side::Buy, 2, size_limit2, price, arrival2);

        WHEN("a buy market order is submitted") {
            book.market(Side::Sell, 3, size_market, arrival3);
            THEN("the limit_fill and market_fill functions should fire") {
                REQUIRE(100 == book.best_buy());
                REQUIRE(size_limit1 + size_limit2 - size_market == book.volume(price));
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
        auto book = LimitOrderBook();
        book.limit(Side::Buy, 1, size_limit1, price, arrival1);
        book.limit(Side::Buy, 2, size_limit2, price, arrival2);

        WHEN("a buy market order is submitted") {
            book.market(Side::Sell, 3, size_market, arrival3);
            THEN("the limit_fill and market_fill functions should fire") {
                REQUIRE(0 == book.best_buy());
                REQUIRE(0 == book.volume(price));
            }
        }
    }
}

// ---------------------------------------------------------------------------
// MARK: clear
// ---------------------------------------------------------------------------

SCENARIO(R"(LOB
clear()
tree shape (single node):
*
)") {
    GIVEN("an order book and an ID for a single sell order") {
        auto book = LimitOrderBook();
        auto side = Side::Sell;
        uint32_t size = 50;
        uint64_t price = 3253;
        uint64_t arrival = 0x1122334455667788;
        book.limit(side, 1, size, price, arrival);
        WHEN("the book is cleared") {
            book.clear();
            THEN("the book is cleared") {
                REQUIRE(0 == book.volume(price));
                REQUIRE(0 == book.volume_buy());
                REQUIRE(0 == book.volume_sell());
                REQUIRE(0 == book.volume());
                REQUIRE(0 == book.best_buy());
                REQUIRE(0 == book.best_sell());
            }
        }
    }
    GIVEN("an order book and an ID for a single buy order") {
        auto book = LimitOrderBook();
        auto side = Side::Buy;
        uint32_t size = 50;
        uint64_t price = 3253;
        uint64_t arrival = 0x1122334455667788;
        book.limit(side, 1, size, price, arrival);
        WHEN("the book is cleared") {
            book.clear();
            THEN("the book is cleared") {
                REQUIRE(0 == book.volume(price));
                REQUIRE(0 == book.volume_buy());
                REQUIRE(0 == book.volume_sell());
                REQUIRE(0 == book.volume());
                REQUIRE(0 == book.best_buy());
                REQUIRE(0 == book.best_sell());
            }
        }
    }
}

SCENARIO(R"(LOB
clear()
tree shape:
  1
 / \
0   2
)") {
    uint32_t size = 50;
    uint64_t prices[3] = {1, 2, 3};
    uint64_t arrivals[4] = {1, 2, 3, 4};
    GIVEN("an order book with V shaped limit tree (sell)") {
        auto book = LimitOrderBook();
        auto side = Side::Sell;
        // submit the MIDDLE order first (price-wise)
        book.limit(side, 1, size, prices[1], arrivals[0]);
        // the lowest price will be the left child
        book.limit(side, 2, size, prices[0], arrivals[1]);
        // the highest price will be the right child
        book.limit(side, 3, size, prices[2], arrivals[2]);
        WHEN("the book is cleared") {
            book.clear();
            THEN("the book is cleared") {
                REQUIRE(0 == book.volume_buy());
                REQUIRE(0 == book.volume_sell());
                REQUIRE(0 == book.volume());
                REQUIRE(0 == book.best_buy());
                REQUIRE(0 == book.best_sell());
            }
        }
    }
    GIVEN("an order book with V shaped limit tree (buy)") {
        auto book = LimitOrderBook();
        auto side = Side::Buy;
        // submit the MIDDLE order first (price-wise)
        book.limit(side, 1, size, prices[1], arrivals[0]);
        // the lowest price will be the left child
        book.limit(side, 2, size, prices[0], arrivals[1]);
        // the highest price will be the right child
        book.limit(side, 3, size, prices[2], arrivals[2]);
        WHEN("the book is cleared") {
            book.clear();
            THEN("the book is cleared") {
                REQUIRE(0 == book.volume_buy());
                REQUIRE(0 == book.volume_sell());
                REQUIRE(0 == book.volume());
                REQUIRE(0 == book.best_buy());
                REQUIRE(0 == book.best_sell());
            }
        }
    }
}


SCENARIO(R"(LOB
clear()
tree shape:
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
        auto side = Side::Sell;
        book.limit(side, 1, size, prices[0], arrivals[0]);
        book.limit(side, 2, size, prices[1], arrivals[1]);
        book.limit(side, 3, size, prices[2], arrivals[2]);
        WHEN("the book is cleared") {
            book.clear();
            THEN("the book is cleared") {
                REQUIRE(0 == book.volume_buy());
                REQUIRE(0 == book.volume_sell());
                REQUIRE(0 == book.volume());
                REQUIRE(0 == book.best_buy());
                REQUIRE(0 == book.best_sell());
            }
        }
    }
    GIVEN("an order book with right leg shaped limit tree (buy)") {
        auto book = LimitOrderBook();
        auto side = Side::Buy;
        book.limit(side, 1, size, prices[0], arrivals[0]);
        book.limit(side, 2, size, prices[1], arrivals[1]);
        book.limit(side, 3, size, prices[2], arrivals[2]);
        WHEN("the book is cleared") {
            book.clear();
            THEN("the book is cleared") {
                REQUIRE(0 == book.volume_buy());
                REQUIRE(0 == book.volume_sell());
                REQUIRE(0 == book.volume());
                REQUIRE(0 == book.best_buy());
                REQUIRE(0 == book.best_sell());
            }
        }
    }
}

SCENARIO(R"(LOB
clear()
tree shape:
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
        auto side = Side::Sell;
        book.limit(side, 1, size, prices[2], arrivals[0]);
        book.limit(side, 2, size, prices[1], arrivals[1]);
        book.limit(side, 3, size, prices[0], arrivals[2]);
        WHEN("the book is cleared") {
            book.clear();
            THEN("the book is cleared") {
                REQUIRE(0 == book.volume_buy());
                REQUIRE(0 == book.volume_sell());
                REQUIRE(0 == book.volume());
                REQUIRE(0 == book.best_buy());
                REQUIRE(0 == book.best_sell());
            }
        }
    }
    GIVEN("an order book with left leg shaped limit tree (buy)") {
        auto book = LimitOrderBook();
        auto side = Side::Buy;
        book.limit(side, 1, size, prices[2], arrivals[0]);
        book.limit(side, 2, size, prices[1], arrivals[1]);
        book.limit(side, 3, size, prices[0], arrivals[2]);
        WHEN("the book is cleared") {
            book.clear();
            THEN("the book is cleared") {
                REQUIRE(0 == book.volume_buy());
                REQUIRE(0 == book.volume_sell());
                REQUIRE(0 == book.volume());
                REQUIRE(0 == book.best_buy());
                REQUIRE(0 == book.best_sell());
            }
        }
    }
}

SCENARIO(R"(LOB
clear()
tree shape:
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
        auto side = Side::Buy;
        // submit the MIDDLE order first (price-wise)
        book.limit(side, 1, size, prices[1], arrivals[0]);
        // the lowest price will be the left child
        book.limit(side, 2, size, prices[0], arrivals[1]);
        // the highest price will be the right child
        book.limit(side, 3, size, prices[2], arrivals[2]);
        // the last price will be the left branch of the right child
        book.limit(side, 4, size, prices[3], arrivals[3]);
        WHEN("the book is cleared") {
            book.clear();
            THEN("the book is cleared") {
                REQUIRE(0 == book.volume_buy());
                REQUIRE(0 == book.volume_sell());
                REQUIRE(0 == book.volume());
                REQUIRE(0 == book.best_buy());
                REQUIRE(0 == book.best_sell());
            }
        }
    }
}

SCENARIO(R"(LOB
clear()
tree shape:
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
        auto side = Side::Buy;
        // submit the MIDDLE order first (price-wise)
        book.limit(side, 1, size, prices[1], arrivals[0]);
        // the lowest price will be the left child
        book.limit(side, 2, size, prices[0], arrivals[1]);
        // the highest price will be the right child
        book.limit(side, 3, size, prices[2], arrivals[2]);
        // the last price will be the left branch of the right child
        book.limit(side, 4, size, prices[3], arrivals[3]);
        // the last price will be the left branch of the right child
        book.limit(side, 5, size, prices[4], arrivals[4]);
        WHEN("the book is cleared") {
            book.clear();
            THEN("the book is cleared") {
                REQUIRE(0 == book.volume_buy());
                REQUIRE(0 == book.volume_sell());
                REQUIRE(0 == book.volume());
                REQUIRE(0 == book.best_buy());
                REQUIRE(0 == book.best_sell());
            }
        }
    }
}

SCENARIO(R"(LOB
clear()
tree shape:
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
        auto side = Side::Buy;
        book.limit(side, 1, size, prices[0], arrivals[0]);
        book.limit(side, 2, size, prices[1], arrivals[1]);
        book.limit(side, 3, size, prices[2], arrivals[2]);
        book.limit(side, 4, size, prices[3], arrivals[3]);
        WHEN("the book is cleared") {
            book.clear();
            THEN("the book is cleared") {
                REQUIRE(0 == book.volume_buy());
                REQUIRE(0 == book.volume_sell());
                REQUIRE(0 == book.volume());
                REQUIRE(0 == book.best_buy());
                REQUIRE(0 == book.best_sell());
            }
        }
    }
}

SCENARIO(R"(LOB
clear()
tree shape:
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
        auto side = Side::Sell;
        book.limit(side, 1, size, prices[0], arrivals[0]);
        book.limit(side, 2, size, prices[1], arrivals[1]);
        book.limit(side, 3, size, prices[2], arrivals[2]);
        book.limit(side, 4, size, prices[3], arrivals[3]);
        WHEN("the book is cleared") {
            book.clear();
            THEN("the book is cleared") {
                REQUIRE(0 == book.volume_buy());
                REQUIRE(0 == book.volume_sell());
                REQUIRE(0 == book.volume());
                REQUIRE(0 == book.best_buy());
                REQUIRE(0 == book.best_sell());
            }
        }
    }
}


// order removal

SCENARIO("clear book with queue of orders at limit") {
    GIVEN("an order book and a Limit queue of sell orders") {
        auto book = LimitOrderBook();
        auto side = Side::Sell;
        uint32_t sizeA = 50;
        uint32_t sizeB = 40;
        uint32_t sizeC = 30;
        uint64_t price = 3253;
        uint64_t arrivalA = 0;
        uint64_t arrivalB = 1;
        uint64_t arrivalC = 2;
        book.limit(side, 1, sizeA, price, arrivalA);
        book.limit(side, 2, sizeB, price, arrivalB);
        book.limit(side, 3, sizeC, price, arrivalC);
        WHEN("the book is cleared") {
            book.clear();
            THEN("the book is cleared") {
                REQUIRE(0 == book.volume_buy());
                REQUIRE(0 == book.volume_sell());
                REQUIRE(0 == book.volume());
                REQUIRE(0 == book.best_buy());
                REQUIRE(0 == book.best_sell());
            }
        }
    }
    GIVEN("an order book and a Limit queue of buy orders") {
        auto book = LimitOrderBook();
        auto side = Side::Buy;
        uint32_t sizeA = 50;
        uint32_t sizeB = 40;
        uint32_t sizeC = 30;
        uint64_t price = 3253;
        uint64_t arrivalA = 0;
        uint64_t arrivalB = 1;
        uint64_t arrivalC = 2;
        book.limit(side, 1, sizeA, price, arrivalA);
        book.limit(side, 2, sizeB, price, arrivalB);
        book.limit(side, 3, sizeC, price, arrivalC);
        WHEN("the book is cleared") {
            book.clear();
            THEN("the book is cleared") {
                REQUIRE(0 == book.volume_buy());
                REQUIRE(0 == book.volume_sell());
                REQUIRE(0 == book.volume());
                REQUIRE(0 == book.best_buy());
                REQUIRE(0 == book.best_sell());
            }
        }
    }
}
