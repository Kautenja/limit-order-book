// Reproduce the technical report's state checks and controlled timing study.
//
// Copyright (c) 2026 Christian Kauten
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//

#include "limit_order_book.hpp"
#include "limit_order_book_c.h"
#include <algorithm>
#include <chrono>
#include <cstdint>
#include <iostream>
#include <numeric>
#include <stdexcept>
#include <string>
#include <vector>

using namespace LOB;

/// A monotonic wall clock for batch timing.
using Clock = std::chrono::steady_clock;

/// @brief Fail even in builds with NDEBUG enabled.
/// @param condition the property that must hold
/// @param message the diagnostic for a failed property
inline void check(bool condition, const char* message) {
    if (!condition) throw std::runtime_error(message);
}

/// A study-owned book that explicitly releases allocated price levels.
struct StudyBook : LimitOrderBook {
    /// Release levels before the base class destroys the order map.
    ~StudyBook() { clear(); }
};

/// A portable deterministic generator; modulo selection is intentional.
struct Generator {
    /// Nonzero xorshift64* state.
    uint64_t state;
    /// @brief Initialize a deterministic event stream.
    /// @param seed the nonzero initial state
    explicit Generator(uint64_t seed) : state(seed) { }
    /// @brief Draw a 64-bit value.
    /// @returns the next xorshift64* value
    uint64_t next() {
        state ^= state >> 12;
        state ^= state << 25;
        state ^= state >> 27;
        return state * UINT64_C(2685821657736338717);
    }
};

/// A value-only order for the independent, arrival-ordered vector model.
struct ReferenceOrder {
    /// The active order identifier.
    UID uid;
    /// The resting side.
    Side side;
    /// The unfilled quantity.
    Quantity quantity;
    /// The integer limit price.
    Price price;
};

/// A deliberately simple model with no tree, hash table, or intrusive links.
struct ReferenceBook {
    /// Live orders in arrival order; erasure preserves the remaining order.
    std::vector<ReferenceOrder> orders;

    /// @brief Find an order by a linear scan.
    /// @param uid the identifier to find
    /// @returns its index, or orders.size() when absent
    size_t find(UID uid) const {
        for (size_t i = 0; i < orders.size(); ++i)
            if (orders[i].uid == uid) return i;
        return orders.size();
    }

    /// @brief Match in price order, retaining the first arrival on ties.
    /// @param incoming the mutable incoming order; price zero means market
    void match(ReferenceOrder& incoming) {
        while (incoming.quantity != 0) {
            size_t best = orders.size();
            for (size_t i = 0; i < orders.size(); ++i) {
                const auto& candidate = orders[i];
                if (candidate.side == incoming.side) continue;
                if (incoming.price != 0 &&
                    ((incoming.side == Side::Buy && candidate.price > incoming.price) ||
                     (incoming.side == Side::Sell && candidate.price < incoming.price)))
                    continue;
                if (best == orders.size() ||
                    (incoming.side == Side::Buy && candidate.price < orders[best].price) ||
                    (incoming.side == Side::Sell && candidate.price > orders[best].price))
                    best = i;
            }
            if (best == orders.size()) return;
            const auto filled = std::min(incoming.quantity, orders[best].quantity);
            incoming.quantity -= filled;
            orders[best].quantity -= filled;
            if (orders[best].quantity == 0) orders.erase(orders.begin() + best);
        }
    }

    /// @brief Cross a new limit and rest any remaining quantity.
    /// @param order the incoming value
    void limit(ReferenceOrder order) {
        if (find(order.uid) != orders.size()) return;
        match(order);
        if (order.quantity != 0) orders.push_back(order);
    }
};

/// @brief Compare order fields, aggregates, and FIFO links in the tested domain.
/// @param book the implementation under test
/// @param reference the independent model
void compare(StudyBook& book, const ReferenceBook& reference) {
    Count counts[2] = {0, 0};
    Volume volumes[2] = {0, 0};
    Price bests[2] = {0, 0};
    Volume price_volumes[2][41] = {};
    Count price_counts[41] = {};
    const Order* previous[2][41] = {};
    for (const auto& expected : reference.orders) {
        check(book.has(expected.uid), "missing live ID");
        const auto& actual = book.get(expected.uid);
        check(actual.uid == expected.uid && actual.side == expected.side &&
              actual.price == expected.price &&
              actual.quantity == expected.quantity, "order state differs");
        const auto side = static_cast<unsigned>(expected.side);
        const auto index = expected.price - 80;
        const auto prior = previous[side][index];
        check(actual.limit != nullptr && actual.limit->key == expected.price,
              "containing level differs");
        check(actual.prev == prior, "FIFO predecessor differs");
        if (prior != nullptr) {
            check(prior->next == &actual && prior->limit == actual.limit,
                  "FIFO successor or level differs");
        } else {
            check(actual.limit->order_head == &actual, "FIFO head differs");
        }
        previous[side][index] = &actual;
        ++counts[side];
        volumes[side] += expected.quantity;
        price_volumes[side][expected.price - 80] += expected.quantity;
        ++price_counts[expected.price - 80];
        if (bests[side] == 0 || (side == 0 && expected.price < bests[side]) ||
            (side == 1 && expected.price > bests[side])) bests[side] = expected.price;
    }
    for (UID uid = 1; uid <= 256; ++uid)
        check(book.has(uid) == (reference.find(uid) != reference.orders.size()),
              "live ID set differs");
    check(book.count_sell() == counts[0] && book.count_buy() == counts[1] &&
          book.count() == reference.orders.size(), "order count differs");
    check(book.volume_sell() == volumes[0] && book.volume_buy() == volumes[1] &&
          book.volume() == volumes[0] + volumes[1], "volume differs");
    check(book.best_sell() == bests[0] && book.best_buy() == bests[1],
          "best price differs");
    for (Price price = 80; price <= 120; ++price) {
        const auto index = price - 80;
        check(book.volume_sell(price) == price_volumes[0][index] &&
              book.volume_buy(price) == price_volumes[1][index] &&
              book.count_at(price) == price_counts[index], "price level differs");
        for (unsigned side = 0; side < 2; ++side) {
            const auto tail = previous[side][index];
            if (tail != nullptr)
                check(tail->next == nullptr && tail->limit->order_tail == tail,
                      "FIFO tail differs");
        }
    }
}

/// @brief Replay one deterministic stream and validate after every event.
/// @param seed the stream seed
/// @param events the number of generated events
void validate(uint64_t seed, size_t events) {
    StudyBook book;
    ReferenceBook reference;
    Generator generator(seed);
    size_t selections[5] = {};
    size_t empty_updates = 0;
    size_t duplicate_limits = 0;
    for (size_t event = 0; event < events; ++event) {
        const auto kind = generator.next() % 100;
        UID uid = 1 + generator.next() % 256;
        const Side side = generator.next() % 2 ? Side::Buy : Side::Sell;
        const Quantity quantity = static_cast<Quantity>(1 + generator.next() % 50);
        const Price price = 80 + generator.next() % 41;
        if (kind >= 65 && kind < 99 && !reference.orders.empty())
            uid = reference.orders[generator.next() % reference.orders.size()].uid;
        if (kind < 50) {  // limit, including duplicate live IDs
            ++selections[0];
            if (reference.find(uid) != reference.orders.size()) ++duplicate_limits;
            book.limit(side, uid, quantity, price);
            reference.limit({uid, side, quantity, price});
        } else if (kind < 65) {  // market, possibly larger than available depth
            ++selections[1];
            book.market(side, uid, quantity);
            ReferenceOrder order{uid, side, quantity, 0};
            reference.match(order);
        } else if (kind < 99) {  // cancel or reduce an existing order
            ++selections[kind < 85 ? 2 : 3];
            const auto index = reference.find(uid);
            if (index != reference.orders.size()) {
                if (kind < 85) {
                    book.cancel(uid);
                    reference.orders.erase(reference.orders.begin() + index);
                } else {
                    const auto removed = std::min(quantity, reference.orders[index].quantity);
                    book.reduce(uid, removed);
                    reference.orders[index].quantity -= removed;
                    if (reference.orders[index].quantity == 0)
                        reference.orders.erase(reference.orders.begin() + index);
                }
            } else {
                ++empty_updates;
            }
        } else {  // reset state, keeping the event stream deterministic
            ++selections[4];
            book.clear();
            reference.orders.clear();
        }
        try {
            compare(book, reference);
        } catch (const std::exception&) {
            std::cerr << "seed=" << seed << " event=" << event << '\n';
            throw;
        }
    }
    std::cout << "{\"seed\":" << seed << ",\"events\":" << events
              << ",\"limit_selections\":" << selections[0]
              << ",\"market_selections\":" << selections[1]
              << ",\"cancel_selections\":" << selections[2]
              << ",\"reduce_selections\":" << selections[3]
              << ",\"clear_selections\":" << selections[4]
              << ",\"empty_cancel_or_reduce\":" << empty_updates
              << ",\"duplicate_limits\":" << duplicate_limits
              << ",\"status\":\"passed\"}\n";
}

/// @brief Construct a reproducible price permutation outside timing.
/// @param size the number of distinct prices
/// @param order ascending, descending, or shuffled
/// @param seed the Fisher-Yates shuffle seed
/// @returns the prices 1 through size, in the requested order
std::vector<Price> prices(size_t size, const std::string& order, uint64_t seed) {
    std::vector<Price> result(size);
    std::iota(result.begin(), result.end(), Price{1});
    if (order == "descending") std::reverse(result.begin(), result.end());
    if (order == "shuffled") {
        Generator generator(seed);
        for (size_t i = size; i > 1; --i)
            std::swap(result[i - 1], result[generator.next() % i]);
    }
    return result;
}

/// @brief Return elapsed wall time for a batch.
/// @param start the batch start
/// @returns elapsed nanoseconds
uint64_t elapsed(Clock::time_point start) {
    return static_cast<uint64_t>(std::chrono::duration_cast<std::chrono::nanoseconds>(
        Clock::now() - start).count());
}

/// @brief Measure a validated workload; setup and cleanup are untimed.
/// @param kind the workload name
/// @param size the number of initial levels or orders per sweep
/// @param seed the permutation seed
/// @param budget the target number of insertions or pair iterations
/// @param emit whether to print this observation (false for process warmup)
void measure(const std::string& kind, size_t size, uint64_t seed, size_t budget,
             bool emit = true) {
    uint64_t nanoseconds = 0;
    uint64_t operations = 0;
    uint64_t checksum = 0;
    const auto order = prices(size, kind, seed);
    if (kind == "clock") {
        for (size_t i = 0; i < budget; ++i) {
            const auto start = Clock::now();
            nanoseconds += elapsed(start);
        }
        operations = budget;
    } else if (kind == "ascending" || kind == "descending" || kind == "shuffled") {
        const auto batches = std::max(size_t{1}, budget / size);
        for (size_t batch = 0; batch < batches; ++batch) {
            StudyBook book;
            const auto start = Clock::now();
            for (size_t i = 0; i < size; ++i)
                book.limit_buy(i + 1, 10, order[i]);
            nanoseconds += elapsed(start);
            check(book.count() == size && book.volume() == size * 10 &&
                  book.best_buy() == size, "insertion batch failed");
            checksum += book.count();
            operations += size;
        }
    } else if (kind == "existing_cpp" || kind == "existing_c") {
        const auto initial = prices(size, "shuffled", seed);
        if (kind == "existing_cpp") {
            StudyBook book;
            for (auto price : initial) book.limit_buy(price, 10, price);
            const auto start = Clock::now();
            for (size_t i = 0; i < budget; ++i) {
                book.limit_buy(size + 1, 1, 1);
                book.cancel(size + 1);
            }
            nanoseconds = elapsed(start);
            check(book.count() == size && book.volume() == size * 10 &&
                  !book.has(size + 1), "C++ existing-level pair failed");
            checksum = book.volume();
        } else {
            auto book = ::new_();
            for (auto price : initial) ::limit_buy(book, price, 10, price);
            const auto start = Clock::now();
            for (size_t i = 0; i < budget; ++i) {
                ::limit_buy(book, size + 1, 1, 1);
                ::cancel(book, size + 1);
            }
            nanoseconds = elapsed(start);
            check(::count(book) == size && ::volume(book) == size * 10 &&
                  !::has(book, size + 1), "C existing-level pair failed");
            checksum = ::volume(book);
            ::clear(book);
            ::delete_(book);
        }
        operations = budget;  // one add-plus-cancel pair, not one API call
    } else if (kind == "fifo_sweep" || kind == "level_sweep") {
        const auto initial = prices(size, "shuffled", seed);
        const auto batches = std::max(size_t{1}, budget / size);
        for (size_t batch = 0; batch < batches; ++batch) {
            StudyBook book;
            for (size_t i = 0; i < size; ++i)
                book.limit_sell(i + 1, 10, kind == "fifo_sweep" ? 1 : initial[i]);
            const auto start = Clock::now();
            book.market_buy(size + 1, static_cast<Quantity>(size * 10));
            nanoseconds += elapsed(start);
            check(book.count() == 0 && book.volume() == 0 && book.best_sell() == 0,
                  "sweep failed");
            checksum += size;
            ++operations;  // one market request, regardless of fills
        }
    } else {
        throw std::runtime_error("unknown workload");
    }
    if (emit)
        std::cout << kind << ',' << size << ',' << seed << ',' << operations << ','
                  << nanoseconds << ',' << checksum << '\n';
}

/// @brief Run one check stream or one timing observation in a fresh process.
/// @param argc the argument count
/// @param argv check seed events, or workload size seed budget
/// @returns zero on success, nonzero on validation or argument failure
int main(int argc, char** argv) {
    try {
        if (argc == 4 && std::string(argv[1]) == "check") {
            validate(std::stoull(argv[2]), std::stoull(argv[3]));
        } else if (argc == 5) {
            const auto size = std::stoull(argv[2]);
            const auto seed = std::stoull(argv[3]);
            const auto budget = std::stoull(argv[4]);
            check(size > 0 && size <= 100000 && seed > 0 && budget > 0,
                  "invalid workload arguments");
            measure(argv[1], size, seed, std::min(budget, 4096ULL), false);
            measure(argv[1], size, seed, budget);
        } else {
            throw std::runtime_error("usage: study check seed events | workload size seed budget");
        }
    } catch (const std::exception& error) {
        std::cerr << error.what() << '\n';
        return 1;
    }
}
