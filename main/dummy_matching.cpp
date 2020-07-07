// A dummy PITCH Client that prints to the console.
// Copyright 2019 Christian Kauten
//

#include <iostream>
#include <sstream>
#include <string>
#include <random>
#include "limit_order_book.hpp"

using namespace LOB;

int main() {
    LimitOrderBook book;
    std::default_random_engine generator;
    auto price = std::normal_distribution<double>(500, 20);
    auto quantity = std::normal_distribution<double>(1000, 100);
    for (int i = 1; i < 10000000; i++) {
        auto price_ = static_cast<uint64_t>(price(generator));
        auto quantity_ = static_cast<uint32_t>(quantity(generator));
        book.limit(Side::Buy, 0, 100, price_);
        if (i % 11 == 0) {  // randomly submit a market order
            book.market(Side::Sell, 1, quantity_);
        }
    }
}
