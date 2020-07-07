// Benchmark the Limit Order Book (LOB).
// Copyright 2019 Christian Kauten
//

#define CATCH_CONFIG_MAIN
#define CATCH_CONFIG_ENABLE_BENCHMARKING
#include <random>
#include <vector>
#include <unordered_set>
#include <string>
#include <sstream>
#include <iostream>
#include "catch.hpp"
#include "limit_order_book.hpp"

using namespace LOB;
using Catch::Benchmark::Chronometer;

//
// MARK: new limits
//

inline void spam_limits(LimitOrderBook& book, int count) {
    for (int i = 0; i < count; i++) book.limit(Side::Buy, i, 50, i);
}

TEST_CASE("Spam new Limits") {
    BENCHMARK_ADVANCED("send 1 new limits")(Chronometer cron) {
        auto book = LimitOrderBook();
        cron.measure([&]() {
            spam_limits(book, 1);
        });
    };
    BENCHMARK_ADVANCED("send 10 new limits")(Chronometer cron) {
        auto book = LimitOrderBook();
        cron.measure([&]() {
            spam_limits(book, 10);
        });
    };
    BENCHMARK_ADVANCED("send 100 new limits")(Chronometer cron) {
        auto book = LimitOrderBook();
        cron.measure([&]() {
            spam_limits(book, 100);
        });
    };
    BENCHMARK_ADVANCED("send 1000 new limits")(Chronometer cron) {
        auto book = LimitOrderBook();
        cron.measure([&]() {
            spam_limits(book, 1000);
        });
    };
}

//
// MARK: new orders
//

inline void spam_orders(LimitOrderBook& book, int count, int variance = 5) {
    for (int i = 0; i < count; i++)
        book.limit(Side::Buy, i, 50, i % variance);
}

TEST_CASE("Spam new Orders") {
    BENCHMARK_ADVANCED("send 1 orders")(Chronometer cron) {
        auto book = LimitOrderBook();
        cron.measure([&]() {
            spam_orders(book, 1);
        });
    };
    BENCHMARK_ADVANCED("send 10 orders")(Chronometer cron) {
        auto book = LimitOrderBook();
        cron.measure([&]() {
            spam_orders(book, 10);
        });
    };
    BENCHMARK_ADVANCED("send 100 orders")(Chronometer cron) {
        auto book = LimitOrderBook();
        cron.measure([&]() {
            spam_orders(book, 100);
        });
    };
    BENCHMARK_ADVANCED("send 1000 orders")(Chronometer cron) {
        auto book = LimitOrderBook();
        cron.measure([&]() {
            spam_orders(book, 1000);
        });
    };
}

//
// MARK: Random submission and cancellation
//

inline void spam_orders_random_cancels(
    LimitOrderBook& book,
    int count,
    int mean = 500,
    int variance = 30,
    int cancel_every = 5
) {
    auto generator = std::default_random_engine();
    auto price_distribution = std::normal_distribution<double>(mean, variance);
    book.limit(Side::Buy, 0, 50, price_distribution(generator));
    for (int i = 1; i < count; i++) {
        book.limit(Side::Buy, i, 50, price_distribution(generator));
        if (i % cancel_every == 0)
            book.cancel(i - cancel_every);
    }
}

TEST_CASE("Spam orders and randomly cancel orders") {
    BENCHMARK_ADVANCED("send and cancel 10 orders")(Chronometer cron) {
        auto book = LimitOrderBook();
        cron.measure([&]() {
            spam_orders_random_cancels(book, 10);
        });
    };
    BENCHMARK_ADVANCED("send and cancel 100 orders")(Chronometer cron) {
        auto book = LimitOrderBook();
        cron.measure([&]() {
            spam_orders_random_cancels(book, 100);
        });
    };
    BENCHMARK_ADVANCED("send and cancel 1000 orders")(Chronometer cron) {
        auto book = LimitOrderBook();
        cron.measure([&]() {
            spam_orders_random_cancels(book, 1000);
        });
    };
    BENCHMARK_ADVANCED("send and cancel 10000 orders")(Chronometer cron) {
        auto book = LimitOrderBook();
        cron.measure([&]() {
            spam_orders_random_cancels(book, 10000);
        });
    };
}

//
// MARK: Random submission, cancellation, and market orders
//

inline void spam_limit_random_orders(
    LimitOrderBook& book,
    int count,
    int price_mean = 500,
    int price_variance = 20,
    int quantity_mean = 100,
    int quantity_variance = 10,
    int order_every = 100
) {
    auto generator = std::default_random_engine();
    auto price = std::normal_distribution<double>(price_mean, price_variance);
    auto quantity = std::normal_distribution<double>(quantity_mean, quantity_variance);
    for (int i = 1; i < count; i++) {
        auto price_ = static_cast<uint64_t>(price(generator));
        auto quantity_ = static_cast<uint32_t>(quantity(generator));
        book.limit(Side::Buy, i, 100, price_);
        if (i % order_every == 0)  // random submit a market order
            book.market(Side::Sell, i, quantity_);
    }
}

TEST_CASE("Spam limit orders and occasional market orders") {
    BENCHMARK("1000 limit orders") {
        auto book = LimitOrderBook();
        spam_limit_random_orders(book, 1000);
    };
    BENCHMARK("10000 limit orders") {
        auto book = LimitOrderBook();
        spam_limit_random_orders(book, 10000);
    };
    BENCHMARK("100000 limit orders") {
        auto book = LimitOrderBook();
        spam_limit_random_orders(book, 100000);
    };
}

inline void spam_limit_many_market_orders(
    LimitOrderBook& book,
    int count,
    int price_mean = 500,
    int price_variance = 20,
    int quantity_mean = 50,
    int quantity_variance = 10
) {
    auto generator = std::default_random_engine();
    auto price = std::normal_distribution<double>(price_mean, price_variance);
    auto quantity = std::normal_distribution<double>(quantity_mean, quantity_variance);
    for (int i = 1; i < count; i++) {
        auto price_ = static_cast<uint64_t>(price(generator));
        auto quantity_ = static_cast<uint32_t>(quantity(generator));
        book.limit(Side::Buy, i, 100, price_);
        book.market(Side::Sell, i, quantity_);
    }
}

TEST_CASE("Spam limit orders and constant market orders") {
    BENCHMARK("10 limit orders") {
        auto book = LimitOrderBook();
        spam_limit_many_market_orders(book, 10);
    };
    BENCHMARK("100 limit orders") {
        auto book = LimitOrderBook();
        spam_limit_many_market_orders(book, 100);
    };
    BENCHMARK("1000 limit orders") {
        auto book = LimitOrderBook();
        spam_limit_many_market_orders(book, 1000);
    };
    BENCHMARK("10000 limit orders") {
        auto book = LimitOrderBook();
        spam_limit_many_market_orders(book, 10000);
    };
}
