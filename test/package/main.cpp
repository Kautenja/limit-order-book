// A consumer program that matches two orders through the installed headers.
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

// Simulate unrelated consumer headers using the old standalone names and guards.
#define BINARY_SEARCH_TREE_HPP
#define DOUBLY_LINKED_LIST_HPP

namespace BST {
/// @brief An unrelated consumer's tree type.
struct Node;
}  // namespace BST

namespace DLL {
/// @brief An unrelated consumer's list type.
struct Node;
}  // namespace DLL

#include "binary_search_tree.hpp"
#include "doubly_linked_list.hpp"
#include "limit_order_book.hpp"
#include <type_traits>

#undef BINARY_SEARCH_TREE_HPP
#undef DOUBLY_LINKED_LIST_HPP

static_assert(std::is_base_of<LOB::BST::Node<LOB::Price>, LOB::Limit>::value,
    "Limits must use the project's tree node");
static_assert(std::is_base_of<LOB::DLL::Node, LOB::Order>::value,
    "Orders must use the project's list node");

int main() {
    LOB::LimitOrderBook book;
    book.limit(LOB::Side::Sell, 1, 100, 50);
    book.limit(LOB::Side::Buy, 2, 40, 50);  // crosses and partially fills
    if (book.best_sell() != 50) return 1;
    if (book.volume_sell() != 60) return 1;
    if (book.count() != 1) return 1;
    return 0;
}
