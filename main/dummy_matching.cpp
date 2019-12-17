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
    Account account;
    LimitOrderBook book;
    std::default_random_engine generator;
    auto price = std::normal_distribution<double>(500, 20);
    auto size = std::normal_distribution<double>(1000, 100);
    for (int i = 1; i < 10000000; i++) {
        auto price_ = static_cast<uint64_t>(price(generator));
        auto size_ = static_cast<uint32_t>(size(generator));
        book.limit(&account, Side::Buy, 100, price_, i);
        if (i % 11 == 0) {  // random submit a market order
            book.market(&account, Side::Sell, size_, i);
        }
    }
}
