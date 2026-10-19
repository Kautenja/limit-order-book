# Data Structures

The binary search tree and doubly linked list are first-party components of
`limit-order-book`, maintained under the project's [MIT license](../../LICENSE).
Make fixes here, alongside the matching engine and its tests. Their former
standalone repositories are no longer dependencies or maintenance targets.

## Source And Compatibility

-   [binary_search_tree.hpp](../../include/binary_search_tree.hpp) provides
    `BST::Node`, insertion, removal, and minimum/maximum traversal.
-   [doubly_linked_list.hpp](../../include/doubly_linked_list.hpp) provides
    `DLL::Node` and intrusive queue operations.

Keep these header names, include guards, namespaces, signatures, and node
layouts compatible. `Order` derives from `DLL::Node`, and `Limit` derives from
`BST::Node<Price>`; consumers can observe the inherited fields and types.
The existing global namespaces are a compatibility exception to the C++
style guide's `LOB` convention. The nodes borrow pointers; callers own storage.

This ownership change does not rebalance the tree. Tree traversal and general
insertion/removal cost $O(H)$ in tree height, which can reach $O(M)$ for $M$
limits. Appending through a list tail and unlinking a known list node cost
$O(1)$; `push_back` through only a head pointer and `count` traverse the list.
See the [technical report](../whitepaper/README.md) for book-level complexity.

## Build And Validation

CMake installs both headers with the other first-party headers and discovers
their tests in `test/`. There is no separate package, build system, or upstream
update procedure for either component. `robin-map` remains in `vendor/`.

Run `./main.sh test` for tree, list, matching, and Python coverage. Run
`./main.sh test-package` and `./main.sh deployment` when changing packaging or
public headers. Benchmark before and after changing runtime behavior, using
the same workloads and compiler flags. The tree benchmarks in
[benchmark_bst.cpp](../../benchmark/benchmark_bst.cpp) use Catch2 v3 and own
fresh node storage per measured iteration.

## Historical Evidence

The technical report's archived evidence predates local ownership. Its source
hashes and commands retain the original vendor paths to identify what was
measured. Do not rewrite those records to suggest a new evaluation. Current
source links and the evaluation driver use the first-party paths; a fresh
study records new paths and hashes.
