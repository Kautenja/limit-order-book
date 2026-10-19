// Run each benchmark iteration with an independently owned order book.
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

#ifndef LOB_BENCHMARK_FRESH_BOOKS_HPP_
#define LOB_BENCHMARK_FRESH_BOOKS_HPP_

#include "limit_order_book.hpp"
#include <memory>

namespace LOB {
namespace Benchmark {

/// @brief Own an empty book and release its limit nodes after measurement.
struct BookFixture {
    /// @brief The independently owned book for one measured iteration.
    LimitOrderBook book;

    /// @brief Clear the raw limit nodes before the book's containers are destroyed.
    ~BookFixture() { book.clear(); }
};

/// @brief Measure a workload on a separate empty book for every iteration.
///
/// @details Book construction and cleanup happen outside the measured callback.
/// The fixed array keeps every book alive and at the same address until timing ends.
///
/// @tparam Timer a chronometer with runs() and indexed measure(callback) methods
/// @tparam Workload a callable accepting a mutable LimitOrderBook reference
/// @param timer the chronometer requesting the measured iterations
/// @param workload the order operations to measure on each empty book
///
template<typename Timer, typename Workload>
inline void measure_fresh_books(Timer& timer, Workload workload) {
    auto books = std::make_unique<BookFixture[]>(timer.runs());
    timer.measure([&](int index) {
        workload(books[index].book);
    });
}

}  // namespace Benchmark
}  // namespace LOB

#endif  // LOB_BENCHMARK_FRESH_BOOKS_HPP_
