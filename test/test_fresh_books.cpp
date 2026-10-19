// Verify that benchmark iterations cannot share order book state.
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

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#include "../benchmark/fresh_books.hpp"
#include <cstddef>
#include <vector>

using namespace LOB;

namespace {

/// @brief Exercise repeated callbacks without timing them.
struct RepeatedMeasurement {
    /// @brief The number of callbacks requested for each sample.
    int iterations;

    /// @brief Return the number of measured iterations in one sample.
    ///
    /// @returns the requested callback count
    ///
    int runs() const { return iterations; }

    /// @brief Invoke a measured callback with each iteration's index.
    ///
    /// @tparam Callback a callable accepting an integer iteration index
    /// @param callback the workload to invoke for each iteration
    ///
    template<typename Callback>
    void measure(Callback callback) {
        for (int index = 0; index < iterations; ++index) callback(index);
    }
};

}  // namespace

SCENARIO("benchmark iterations use independent empty books", "[benchmark]") {
    GIVEN("a chronometer that repeats the measured workload") {
        const auto iterations = GENERATE(1, 3, 8);
        RepeatedMeasurement timer{iterations};
        int calls = 0;
        WHEN("two samples submit and match the same order IDs") {
            for (int sample = 0; sample < 2; ++sample) {
                std::vector<const LimitOrderBook*> measured_books;
                Benchmark::measure_fresh_books(timer, [&](LimitOrderBook& book) {
                    REQUIRE(book.count() == 0);
                    REQUIRE(book.volume() == 0);
                    REQUIRE(book.best_sell() == 0);
                    REQUIRE(book.best_buy() == 0);
                    REQUIRE_FALSE(book.has(7));
                    REQUIRE_FALSE(book.has(8));
                    for (const auto previous : measured_books)
                        REQUIRE(previous != &book);
                    measured_books.push_back(&book);

                    book.limit_sell(7, 10, 100);
                    book.limit_sell(8, 20, 100);
                    book.market_buy(9, 5);
                    REQUIRE(book.count() == 2);
                    REQUIRE(book.volume() == 25);
                    REQUIRE(book.get(7).quantity == 5);
                    REQUIRE(book.get(8).quantity == 20);
                    ++calls;
                });
                REQUIRE(measured_books.size() == static_cast<std::size_t>(iterations));
            }
            THEN("every iteration executes the complete workload") {
                REQUIRE(calls == 2 * iterations);
            }
        }
    }
}
