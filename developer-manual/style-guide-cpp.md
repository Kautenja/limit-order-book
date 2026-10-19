# C++ Style Guide

This guide is for agents and humans writing or refactoring the C++ library in
`include/`, the C API, the Catch2 tests, the benchmarks, and the executables
in `main/`. [AGENTS.md](../AGENTS.md) owns workflow, architecture, and
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
-   `benchmark/benchmark_lob.cpp` and `main/dummy_matching.cpp` carry a short
    `// Copyright 2019 Christian Kauten` line. Replace it with the full header,
    keeping the original year, when a requested edit touches the file.
-   `include/lib_lob.hpp` and `src/lib_lob.cpp` spell `EXPRESS` as
    `EXTERNRESS`. Correct it when a requested edit touches either file.
-   Never add this header to `vendor/` or `notes/`.

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

-   `include/` is a header-only library. Users copy it into their projects or
    add it as a submodule, so new library code lives in headers and is marked
    `inline` or templated. `src/lib_lob.cpp` only compiles the C API.
-   SCons builds with `-std=c++17`. The PyPI extension in `pyproject.toml`
    uses the compiler default, which is C++14 for MSVC. Library code must
    compile as C++14 for the Windows wheels, so avoid C++17-only features in
    `include/` such as `if constexpr`, structured bindings, `std::optional`,
    and inline variables. Tests, benchmarks, and `main/` may use C++17.
-   Code must compile without warnings under `-pedantic -Wall` with both
    `g++` and Apple Clang.
-   Use the `LOB` namespace for everything in `include/` except the C API.
-   Use include guards named after the file, such as `LOB_LIMIT_TREE_HPP_`,
    matching the existing headers. Do not mix in `#pragma once`.
-   Use `.hpp` for headers and `.cpp` for translation units.
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
-   **Macros:** `UPPER_SNAKE_CASE`, such as `EXTERN`, and `#undef` them at the
    end of the header that defines them.

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
    EXTERN void limit_sell(
        LOB::LimitOrderBook* book,
        LOB::UID uid,
        LOB::Quantity quantity,
        LOB::Price price
    ) { book->limit_sell(uid, quantity, price); }
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
-   In tests, define `CATCH_CONFIG_MAIN` and include `catch.hpp` before the
    project header.
-   `using namespace LOB;` is acceptable in tests, benchmarks, and `main/`.
    Never put `using namespace` in a header.

## Documentation

`Doxyfile` builds the API reference from the headers into `docs/`, so the
header comments are the published documentation. Write them for a reader who
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

`include/lib_lob.hpp` is the boundary that Python and C users see.

-   Wrap functions in `extern "C"` and mark each with `EXTERN`.
-   Mirror `LimitOrderBook` methods by name. Name functions that collide with
    keywords with a trailing underscore: `new_` and `delete_`.
-   Pass the book as the first parameter, `LOB::LimitOrderBook* book`.
-   Use fixed-width types through the `LOB` aliases. Never pass references,
    templates, or standard library types across the boundary.
-   Do not let new functions throw across the boundary. An exception that
    escapes an `extern "C"` function called from `ctypes` aborts Python. Some
    existing functions can throw on unknown IDs; document that, and do not
    copy the pattern.
-   Every C function needs a matching `argtypes` and `restype` declaration in
    `limit_order_book/library.py`, a method in
    `limit_order_book/limit_order_book.py`, and tests in both languages. The
    `ctypes` type must have the same width as the C++ alias.

## Tests And Benchmarks

-   Put tests in `test/test_<header>.cpp`. SCons builds each file in `test/`
    as its own binary with the alias `test/<file>.cpp`.
-   Write tests as Catch2 `SCENARIO`, `GIVEN`, `WHEN`, and `THEN` blocks that
    describe the market event: "a sell limit order crosses the best buy" is
    better than "limit works".
-   Use `REQUIRE` for book state. Check prices, volumes, counts, and best
    prices on both sides after each event.
-   Cover empty books, a single order, several orders at one limit, several
    limits, crossing orders that partially and fully fill, cancels of the
    best and non-best orders, and extreme `Price` and `Quantity` values.
-   Put benchmarks in `benchmark/` with `CATCH_CONFIG_ENABLE_BENCHMARKING`
    and `BENCHMARK_ADVANCED` with a `Chronometer`, so book setup stays outside
    the measured region.
-   Compare benchmarks before and after a hot-path change with the same
    compiler, flags, and workload.

## Adaptation Checklist

Before finishing a C++ change, check:

-   The file starts with the purpose line and the MIT header.
-   Library code is in `LOB`, header-only, and compiles as C++14 and C++17.
-   Signatures use the domain aliases and include units or meaning in names.
-   Every new or changed declaration has `///` documentation, including side,
    complexity, failure behavior, and ownership where they apply.
-   The matching invariants in [AGENTS.md](../AGENTS.md) still hold.
-   C API changes have matching `ctypes` declarations, Python methods, and
    tests in both languages.
-   Catch2 scenarios cover the changed behavior, and hot-path changes have
    before and after benchmark results.
-   No edits were made under `vendor/` or `docs/`.
