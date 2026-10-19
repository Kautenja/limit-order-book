// Benchmark binary search tree insertion and removal.
//
// Copyright (c) 2019 Christian Kauten
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
#include <catch2/benchmark/catch_benchmark.hpp>
#include "binary_search_tree.hpp"
#include <algorithm>
#include <random>
#include <string>
#include <vector>

namespace LOB {
namespace Benchmark {

/// @brief Own a fresh tree and its removal order for one measured iteration.
struct TreeFixture {
    /// @brief Stable storage for the tree's nodes.
    std::vector<BST::Node<double>> nodes;
    /// @brief Shuffled pointers to the owned nodes for removal.
    std::vector<BST::Node<double>*> removal_order;
    /// @brief The root of the current tree, or nullptr when empty.
    BST::Node<double>* root = nullptr;

    /// @brief Prepare detached nodes with increasing or normally distributed keys.
    ///
    /// @param count the number of nodes to prepare
    /// @param random_keys whether to use the upstream normal key distribution
    ///
    TreeFixture(int count, bool random_keys) : nodes(count) {
        auto generator = std::default_random_engine();
        auto distribution = std::normal_distribution<double>(500, 20);
        for (int i = 0; i < count; ++i) {
            nodes[i].key = random_keys ? distribution(generator) : i;
            removal_order.push_back(&nodes[i]);
        }
        std::shuffle(removal_order.begin(), removal_order.end(), generator);
    }

    /// @brief Insert every prepared node in its original order.
    inline void insert() {
        for (auto& node : nodes) BST::insert(&root, &node);
    }

    /// @brief Unlink every node in shuffled order, preserving owned storage.
    inline void remove() {
        for (auto node : removal_order) BST::remove(&root, node);
    }
};

/// @brief Measure insertion or removal using independently owned trees.
///
/// @param timer the chronometer requesting measured iterations
/// @param count the number of nodes per tree
/// @param random_keys whether to generate normally distributed keys
/// @param remove whether to measure removal instead of insertion
/// @details Allocation, key generation, removal shuffle, and cleanup are untimed.
/// Removal starts with a populated tree; insertion starts with detached nodes.
///
inline void measure_trees(
    Catch::Benchmark::Chronometer& timer,
    int count,
    bool random_keys,
    bool remove
) {
    std::vector<TreeFixture> trees;
    trees.reserve(timer.runs());
    for (int i = 0; i < timer.runs(); ++i) {
        trees.emplace_back(count, random_keys);
        if (remove) trees.back().insert();
    }
    if (remove) {
        timer.measure([&](int index) { trees[index].remove(); });
    } else {
        timer.measure([&](int index) { trees[index].insert(); });
    }
    REQUIRE(std::all_of(trees.begin(), trees.end(), [&](const TreeFixture& tree) {
        return (tree.root == nullptr) == remove;
    }));
}

}  // namespace Benchmark
}  // namespace LOB

using Catch::Benchmark::Chronometer;
using LOB::Benchmark::measure_trees;

// ---------------------------------------------------------------------------
// MARK: LOB::BST::insert
// ---------------------------------------------------------------------------

TEST_CASE("LOB::BST::insert nodes with strictly increasing key", "[bst-benchmark]") {
    for (int count : {10, 100, 1000, 10000}) {
        BENCHMARK_ADVANCED(std::to_string(count) + " nodes")(Chronometer timer) {
            measure_trees(timer, count, false, false);
        };
    }
}

TEST_CASE("LOB::BST::insert nodes with random key", "[bst-benchmark]") {
    for (int count : {10, 100, 1000, 10000, 100000}) {
        BENCHMARK_ADVANCED(std::to_string(count) + " nodes")(Chronometer timer) {
            measure_trees(timer, count, true, false);
        };
    }
}

// ---------------------------------------------------------------------------
// MARK: LOB::BST::remove
// ---------------------------------------------------------------------------

TEST_CASE("LOB::BST::remove nodes in random order", "[bst-benchmark]") {
    for (int count : {10, 100, 1000, 10000, 100000}) {
        BENCHMARK_ADVANCED(std::to_string(count) + " nodes")(Chronometer timer) {
            measure_trees(timer, count, true, true);
        };
    }
}
