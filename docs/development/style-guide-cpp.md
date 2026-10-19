# C++ Style Guide

This guide is for agents and humans writing or refactoring the C++ library in
`include/`, the C API, the Catch2 tests, and the benchmarks.
[AGENTS.md](../../AGENTS.md) owns workflow, architecture, and
validation; this file owns C++ code shape.

The existing headers in `include/` are the style authority. The rules below
describe them and adapt the Doxygen and documentation habits from Christian
Kauten's newer C++ projects. Follow the surrounding code when editing, and
keep style changes scoped to the requested work.

## Copyright Header

Every first-party C++ source, header, test, benchmark, and executable begins
with a one-line file purpose, the copyright line, and the MIT permission
notice, followed by a blank line before the include guard or includes:

```cpp
// An order book for managing Limit / Order objects in a continuous double auction.
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
```

-   New files use the year they are created. Do not bump the year on existing
    files as incidental cleanup.
-   Never add this header to `vendor/` or `docs/references/`.

## Style Fingerprint

New code should read like the existing headers rather than generic C++. The
strongest signals are:

-   Small headers with one clear subject: the order and limit structures, one
    side of the book, the full book, or the C API.
-   Plain structs with default member initializers for data, and classes with
    private state for owners of invariants.
-   Domain type aliases (`UID`, `Quantity`, `Price`, `Count`, `Volume`) in
    every signature instead of raw integer types.
-   Compile-time side selection through `template<Side side>` and explicit
    specializations rather than runtime flags or virtual dispatch.
-   `inline` member functions defined in the class body, short enough to read
    without scrolling.
-   A `/// @brief` comment on every type, alias, member, and function, with
    `@param`, `@tparam`, and `@returns` for anything that takes or returns
    a value.
-   Short trailing comments on branches that name the market event, such as
    `// crosses`, `// order filled`, or `// remove the order if there is no
    quantity left`.
-   Dashed `// MARK:` banners separating function families in long headers and
    test files.

## Project Defaults

-   `include/` is a header-only library distributed through the
    `limit_order_book::limit_order_book` CMake target, an installed package,
    or a copy of the headers, so new library code lives in headers and is
    marked `inline` or templated. `src/limit_order_book_c.cpp` defines the
    C API.
-   The headers require C++14, and `CMakeLists.txt` builds every first-party
    target, including tests and benchmarks, as C++14. Avoid C++17
    features such as `if constexpr`, structured bindings, `std::optional`,
    and inline variables.
-   `LOB_BUILD_EXAMPLES` is a deprecated no-op with a default of `OFF`.
    Preserve it for existing CMake configurations; do not add targets under it.
-   Code must compile without warnings under `-Wall -pedantic` with GCC and
    Clang, and under `/W3` with MSVC.
-   A new public header must be added to the install rules only if it lives
    outside `include/`; everything in `include/` is installed. A new vendored
    dependency needs its include directory in `LOB_VENDOR_INCLUDE_DIRS` and
    its license in the install rules.
-   Use the `LOB` namespace for everything in `include/` except the C API.
-   Use include guards named after the file, such as `LOB_LIMIT_TREE_HPP_`,
    matching the existing headers. Do not mix in `#pragma once`.
-   Use `.hpp` for headers and `.cpp` for translation units. The C API
    header, `limit_order_book_c.h`, is the one `.h` file.
-   Prefer cohesive files over line-count limits. The book and tree headers
    are 300-400 lines; that is comfortable. Around 600-700 lines, ask whether
    the file still owns one data structure.

## Naming

Names describe the market concept, not the implementation detail.

-   **Types:** `PascalCase`, such as `LimitOrderBook`, `LimitTree`, `Order`,
    and `Limit`.
-   **Domain aliases:** short names that say what the integer means: `UID`,
    `Quantity`, `Price`, `Count`, `Volume`, and map aliases such as
    `UIDOrderMap` and `PriceLimitMap`. Add a new alias rather than reusing one
    with a different meaning.
-   **Functions, methods, members, and locals:** `snake_case`, such as
    `limit_sell`, `best_buy`, `count_at`, `order_head`, and `last_best_price`.
-   **Enumerations:** `enum class` with an explicit underlying type and
    `PascalCase` values, such as `Side::Buy` and `Side::Sell`.
-   **Shadowed constructor parameters:** a trailing underscore, such as
    `uid_` and `price_`, initializing members of the same name.
-   **Paired operations:** a `_sell` and `_buy` method for each side, plus a
    side-dispatching method without a suffix, such as `limit_sell`,
    `limit_buy`, and `limit`.
-   **Macros:** `UPPER_SNAKE_CASE` with a `LOB_` prefix, such as
    `LOB_C_API`, and `#undef` them at the end of the header that defines
    them.

## Formatting

-   Indent with four spaces. Put opening braces on the declaration or control
    line.
-   Indent access labels by one space: ` private:` and ` public:`.
-   Put private state before public methods, as in `LimitOrderBook`.
-   Close namespaces with two spaces and a comment: `}  // namespace LOB`.
-   Put two spaces before a trailing comment: `if (...) {  // crosses`.
-   Short guards and side-dispatch `switch` cases may stay on one line:

    ```cpp
    if (*highest_buy == nullptr) *highest_buy = limit;
    ```

    Use braces when a branch does more than one thing.

-   Break long signatures with one parameter per line, indented four spaces,
    and the closing `)` at the declaration's indentation:

    ```cpp
    void limit_sell(
        LOB_LimitOrderBook* book,
        LOB_UID uid,
        LOB_Quantity quantity,
        LOB_Price price
    ) { book->book.limit_sell(uid, quantity, price); }
    ```

-   Attach `*` and `&` to the type: `Limit* limit`, `const Order& get(...)`.
-   Use `static_cast` at integer and enum boundaries. Do not rely on implicit
    narrowing between `Quantity`, `Volume`, and `Price`.
-   Separate function families in long headers and tests with this banner:

    ```cpp
    // ---------------------------------------------------------------------------
    // MARK: find_best
    // ---------------------------------------------------------------------------
    ```

## Includes

-   Include only what the file uses, and include the header that owns a type
    directly.
-   List local and vendor headers first, then standard headers, matching the
    existing files.
-   In tests, include `<catch2/catch_test_macros.hpp>` before the project
    header. Catch2 v3 provides `main` through `Catch2::Catch2WithMain`, so do
    not define `CATCH_CONFIG_MAIN`.
-   `using namespace LOB;` is acceptable in tests and benchmarks.
    Never put `using namespace` in a header.

## Documentation

The header comments are the API reference; there is no separate generated
site. Write them for a reader who
will never open the function body.

-   Use `///` comments. Start with `@brief`, leave a blank `///` line before
    the parameter block, and end multi-line blocks with a blank `///` line.
-   Document every type, alias, member, and function, including private state
    and helpers. A one-line `///` comment is enough for an obvious member.
-   Use `@tparam` for template parameters, `@param` for parameters, and
    `@returns` for return values.
-   State the side semantics: which side an order rests on and which side it
    executes against.
-   State units and meaning: prices are integer ticks with no implied scale,
    quantities are shares, and a price of `0` means an empty side.
-   State complexity for operations on the book and trees, such as $O(1)$ or
    $O(\log M)$, and when it is amortized.
-   State failure behavior: `std::out_of_range` from `orders.at` for unknown
    IDs, a thrown string from `reduce`, or a `nullptr` return.
-   State ownership and lifetime for pointers. A pointer from `get` is
    borrowed and becomes invalid when the order fills, is cancelled, or the
    book is cleared.
-   Use `@details` for the algorithm, and keep implementation comments next to
    the lines they explain.

Comment shape to preserve:

```cpp
/// @brief Cancel an existing order in the book.
///
/// @param order_id the ID of the order to cancel
///
inline void cancel(UID order_id) {
```

Avoid comments that restate the code:

```cpp
// set count to zero
count = 0;
```

## Data Structures And Performance

The book exists to be fast. Treat the hot path as the product.

-   Keep add-at-existing-limit, cancel, and execute-at-best $O(1)$ and new
    limits $O(\log M)$. Document any operation that cannot meet that.
-   Keep the intrusive layout: `Order` derives from `DLL::Node` and `Limit`
    from `BST::Node<Price>`. Do not replace them with standard containers of
    values without benchmark evidence.
-   Do not reorder or resize `Order` and `Limit` fields, or remove
    `Limit::_padding`, without benchmark evidence. Their layout is part of
    the cache behavior.
-   Use the existing hash maps (`std::unordered_map` for orders and
    `tsl::robin_map` for limits) unless a benchmark justifies a change.
-   Avoid virtual functions, `std::function`, and heap allocation in the
    matching path beyond the order and limit nodes. Pass callbacks as
    template parameters, as `LimitTree::market` does.
-   Use integer prices and quantities. Never introduce floating-point prices.
-   Guard against unsigned underflow when subtracting quantities and volumes.

## C API

`include/limit_order_book_c.h` is the boundary that C users see.
It is installed only with `LOB_BUILD_C_LIBRARY=ON`, so the header-only
package does not ship a header without its library.

-   Keep the header valid C99 and C++14: `<stdint.h>` and `<stdbool.h>`
    types, `(void)` for empty parameter lists, and an
    `#ifdef __cplusplus` `extern "C"` guard. Never include a C++ header from
    it.
-   Declare functions in the header and define them in
    `src/limit_order_book_c.cpp`. Mark each declaration with `LOB_C_API`,
    which exports it from the shared library. The library hides every other
    symbol.
-   Mirror `LimitOrderBook` methods by name. Name functions that collide with
    keywords with a trailing underscore: `new_` and `delete_`.
-   Pass the book as the first parameter through the opaque handle,
    `LOB_LimitOrderBook* book`. The handle wraps the C++ book in
    `src/limit_order_book_c.cpp`; C code never sees `LOB::` types.
-   Use the `LOB_` C aliases (`LOB_UID`, `LOB_Price`, and so on).
    `src/limit_order_book_c.cpp` asserts that each matches its C++ alias.
    Never pass references, templates, or standard library types across the
    boundary.
-   Never let an exception cross the boundary: it can abort a C program.
    Check preconditions in the definition instead, as `get` returns
    `NULL` and `cancel` ignores unknown IDs.
-   Cover C functions in `test/package/main.c`. Python independently calls
    C++ through Cython; keep equivalent operations and fixed-width types
    consistent with [Python bindings](python-bindings.md).

## Tests And Benchmarks

-   Put tests in `test/test_<header>.cpp`. CMake compiles every `.cpp` in
    `test/` into one `limit_order_book_tests` binary, and `ctest` runs each
    scenario as its own test, so scenario names must be unique across files.
-   Write tests as Catch2 `SCENARIO`, `GIVEN`, `WHEN`, and `THEN` blocks that
    describe the market event: "a sell limit order crosses the best buy" is
    better than "limit works".
-   Use `REQUIRE` for book state. Check prices, volumes, counts, and best
    prices on both sides after each event.
-   Cover empty books, a single order, several orders at one limit, several
    limits, crossing orders that partially and fully fill, cancels of the
    best and non-best orders, and extreme `Price` and `Quantity` values.
-   Put benchmarks in `benchmark/`, include
    `<catch2/benchmark/catch_benchmark.hpp>`, and use `BENCHMARK_ADVANCED`
    with a `Chronometer`, so book setup stays outside the measured region.
-   For mutating workloads, use `measure_fresh_books` from
    [benchmark/fresh_books.hpp](../../benchmark/fresh_books.hpp). It allocates
    one empty book per `Chronometer::runs()` iteration and selects the book
    by the callback's index. Construction and cleanup stay outside timing;
    never reuse one mutated book across iterations or time its reset.
-   Compare benchmarks before and after a hot-path change with the same
    compiler, flags, and workload.

## Adaptation Checklist

Before finishing a C++ change, check:

-   The file starts with the purpose line and the MIT header.
-   Library code is in `LOB`, header-only, and compiles as C++14.
-   Signatures use the domain aliases and include units or meaning in names.
-   Every new or changed declaration has `///` documentation, including side,
    complexity, failure behavior, and ownership where they apply.
-   The matching invariants in [AGENTS.md](../../AGENTS.md) still hold.
-   C API changes have consumer coverage and keep equivalent Python methods
    and tests consistent.
-   Catch2 scenarios cover the changed behavior, and hot-path changes have
    before and after benchmark results.
-   `./main.sh test-package` passes when the change touches `CMakeLists.txt`,
    `cmake/`, or the set of public headers.
-   No edits were made under `vendor/`.
