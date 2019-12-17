// Test cases for the types and order, limit, and account structures.
// Copyright 2019 Christian Kauten
//

#include "catch.hpp"
#include "structures.hpp"

using namespace LOB;

TEST_CASE("should invert Side::Sell") {
    REQUIRE(Side::Buy == !Side::Sell);
}

TEST_CASE("should invert Side::Buy") {
    REQUIRE(Side::Sell == !Side::Buy);
}

// ---------------------------------------------------------------------------
// MARK: Order
// ---------------------------------------------------------------------------

TEST_CASE("initialize default Order") {
    Order order;
    REQUIRE(order.next == nullptr);
    REQUIRE(order.prev == nullptr);
    REQUIRE(order.uid == 0);
    REQUIRE(order.side == Side::Sell);
    REQUIRE(order.size == 0);
    REQUIRE(order.price == 0);
    REQUIRE(order.arrival == 0);
    REQUIRE(order.execution == 0);
    REQUIRE(order.limit == nullptr);
}

SCENARIO("initialize Order") {
    GIVEN("arbitrary legal parameters") {
        Account account;
        UID uid = 5;
        Side side = Side::Buy;
        Size size = 100;
        Price price = 5746;
        Timestamp arrival = 1000000001;
        Timestamp execution = 1000000002;
        WHEN("an Order is initialized without optionals") {
            Order order = {&account, uid, side, size, price};
            THEN("the order is created with parameters") {
                REQUIRE(order.next == nullptr);
                REQUIRE(order.prev == nullptr);
                REQUIRE(order.account == &account);
                REQUIRE(order.uid == uid);
                REQUIRE(order.side == side);
                REQUIRE(order.size == size);
                REQUIRE(order.price == price);
                REQUIRE(order.arrival == 0);
                REQUIRE(order.execution == 0);
                REQUIRE(order.limit == nullptr);
            }
        }
        WHEN("an Order is initialized with optional arrival") {
            Order order = {&account, uid, side, size, price, arrival};
            THEN("the order is created with parameters") {
                REQUIRE(order.next == nullptr);
                REQUIRE(order.prev == nullptr);
                REQUIRE(order.account == &account);
                REQUIRE(order.uid == uid);
                REQUIRE(order.side == side);
                REQUIRE(order.size == size);
                REQUIRE(order.price == price);
                REQUIRE(order.arrival == arrival);
                REQUIRE(order.execution == 0);
                REQUIRE(order.limit == nullptr);
            }
        }
        WHEN("an Order is initialized with optional execution") {
            Order order = {&account, uid, side, size, price, arrival, execution};
            THEN("the order is created with parameters") {
                REQUIRE(order.next == nullptr);
                REQUIRE(order.prev == nullptr);
                REQUIRE(order.account == &account);
                REQUIRE(order.uid == uid);
                REQUIRE(order.side == side);
                REQUIRE(order.size == size);
                REQUIRE(order.price == price);
                REQUIRE(order.arrival == arrival);
                REQUIRE(order.execution == execution);
                REQUIRE(order.limit == nullptr);
            }
        }
    }
}

// ---------------------------------------------------------------------------
// MARK: Limit
// ---------------------------------------------------------------------------

TEST_CASE("initialize default Limit") {
    Limit limit;
    REQUIRE(limit.key == 0);
    REQUIRE(limit.parent == nullptr);
    REQUIRE(limit.left == nullptr);
    REQUIRE(limit.right == nullptr);
    REQUIRE(limit.size == 0);
    REQUIRE(limit.volume == 0);
    REQUIRE(limit.order_head == nullptr);
    REQUIRE(limit.order_tail == nullptr);
}

SCENARIO("initialize Limit") {
    GIVEN("arbitrary legal parameters and a new order") {
        Account account;
        Size size = 100;
        Price price = 5;
        Order order = {&account, 5, Side::Buy, size, price};
        WHEN("a Limit is initialized") {
            Limit limit = {&order};
            THEN("the limit is created with parameters") {
                REQUIRE(limit.key == price);
                REQUIRE(limit.parent == nullptr);
                REQUIRE(limit.left == nullptr);
                REQUIRE(limit.right == nullptr);
                REQUIRE(limit.size == 1);
                REQUIRE(limit.volume == size);
                REQUIRE(limit.order_head == &order);
                REQUIRE(limit.order_tail == &order);
            }
        }
    }
}

// ---------------------------------------------------------------------------
// MARK: Account
// ---------------------------------------------------------------------------

TEST_CASE("initialize default Account") {
    Account account;
    REQUIRE(account.order_count == 0);
    REQUIRE(account.volume == 0);
    REQUIRE(account.shares == 0);
    REQUIRE(account.capital == 0);
}

SCENARIO("initialize Account") {
    GIVEN("arbitrary legal parameters") {
        Size order_count = 5;
        Volume volume = 1250;
        Shares shares = 100;
        Capital capital = 5000;
        WHEN("an Account is initialized") {
            Account account = {order_count, volume, shares, capital};
            THEN("the account is created with parameters") {
                REQUIRE(account.order_count == order_count);
                REQUIRE(account.volume == volume);
                REQUIRE(account.shares == shares);
                REQUIRE(account.capital == capital);
            }
        }
    }
}

SCENARIO("perform actions on account") {
    GIVEN("an account") {
        Account account;
        WHEN("a limit order is placed") {
            Size size = 100;
            Order order = {&account, 0, Side::Buy, size, 5746, 0};
            account.limit(&order);
            THEN("the account is updated") {
                REQUIRE(account.order_count == 1);
                REQUIRE(account.volume == static_cast<Volume>(size));
                REQUIRE(account.shares == 0);
                REQUIRE(account.capital == 0);
            }
        }
        WHEN("a limit order is canceled") {
            Size size = 100;
            Order order = {&account, 0, Side::Buy, size, 5746, 0};
            account.cancel(&order);
            THEN("the account is updated") {
                REQUIRE(account.order_count == -1);
                REQUIRE(account.volume == -static_cast<Volume>(size));
                REQUIRE(account.shares == 0);
                REQUIRE(account.capital == 0);
            }
        }
        WHEN("a limit order is partially filled (Sell)") {
            Size size = 100;
            Price price = 50;
            Order limit = {&account, 0, Side::Sell, size, price, 0};
            Order market = {&account, 0, Side::Buy, size, price, 0};
            account.limit_partial(&limit, &market);
            THEN("the account is updated") {
                REQUIRE(account.order_count == 0);
                REQUIRE(account.volume == -static_cast<Volume>(size));
                REQUIRE(account.shares == -static_cast<Shares>(size));
                REQUIRE(account.capital == price * size);
            }
        }
        WHEN("a limit order is partially filled (Buy)") {
            Size size = 100;
            Price price = 50;
            Order limit = {&account, 0, Side::Buy, size, price, 0};
            Order market = {&account, 0, Side::Sell, size, price, 0};
            account.limit_partial(&limit, &market);
            THEN("the account is updated") {
                REQUIRE(account.order_count == 0);
                REQUIRE(account.volume == -static_cast<Volume>(size));
                REQUIRE(account.shares == static_cast<Shares>(size));
                REQUIRE(account.capital == -price * size);
            }
        }
        WHEN("a limit order is filled (Sell)") {
            Size size = 100;
            Price price = 50;
            Order limit = {&account, 0, Side::Sell, size, price, 0};
            Order market = {&account, 0, Side::Buy, size, price, 0};
            account.limit_fill(&limit, &market);
            THEN("the account is updated") {
                REQUIRE(account.order_count == -1);
                REQUIRE(account.volume == -static_cast<Volume>(size));
                REQUIRE(account.shares == -static_cast<Shares>(size));
                REQUIRE(account.capital == price * size);
            }
        }
        WHEN("a limit order is filled (Buy)") {
            Size size = 100;
            Price price = 50;
            Order limit = {&account, 0, Side::Buy, size, price, 0};
            Order market = {&account, 0, Side::Sell, size, price, 0};
            account.limit_fill(&limit, &market);
            THEN("the account is updated") {
                REQUIRE(account.order_count == -1);
                REQUIRE(account.volume == -static_cast<Volume>(size));
                REQUIRE(account.shares == static_cast<Shares>(size));
                REQUIRE(account.capital == -price * size);
            }
        }
        WHEN("a market order is filled (Sell)") {
            Size size = 100;
            Price price = 50;
            Order limit = {&account, 0, Side::Buy, size, price, 0};
            Order market = {&account, 0, Side::Sell, size, price, 0};
            account.market_fill(&limit, &market);
            THEN("the account is updated") {
                REQUIRE(account.order_count == 0);
                REQUIRE(account.volume == 0);
                REQUIRE(account.shares == -static_cast<Shares>(size));
                REQUIRE(account.capital == price * size);
            }
        }
        WHEN("a market order is filled (Buy)") {
            Size size = 100;
            Price price = 50;
            Order limit = {&account, 0, Side::Sell, size, price, 0};
            Order market = {&account, 0, Side::Buy, size, price, 0};
            account.market_fill(&limit, &market);
            THEN("the account is updated") {
                REQUIRE(account.order_count == 0);
                REQUIRE(account.volume == 0);
                REQUIRE(account.shares == static_cast<Shares>(size));
                REQUIRE(account.capital == -price * size);
            }
        }
    }
}
