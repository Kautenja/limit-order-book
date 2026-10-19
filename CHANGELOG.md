# Changelog

Notable changes to `limit-order-book` are collected here. Entries before
2.0.0 come from the [GitHub Releases][releases] notes; all entries are checked
against the tagged sources and commit history. Dates are the PyPI upload
dates, or the tag date for releases that were not uploaded.

## Unreleased

**Breaking changes:**

-   Raised the minimum Python version to 3.13. Wheels are published for
    CPython 3.13 and 3.14 on Linux x64 and arm64, macOS x64 and arm64, and
    Windows x64.
-   Replaced the C++-only `include/lib_lob.hpp` with the C99 header
    `include/lib_lob.h`, which declares the C API on an opaque
    `LOB_LimitOrderBook` handle and `LOB_` fixed-width aliases.
    `src/lib_lob.cpp` now holds the definitions, so the header can be
    included from any number of C or C++ translation units. The function
    names and ABI are unchanged, but C++ code that called them with a
    `LOB::LimitOrderBook*` must use the handle from `new_`.

**Features:**

-   Added the `LOB_BUILD_C_LIBRARY` CMake option, which builds and installs
    the C API as the versioned `liblimit_order_book` shared library, exported
    as `limit_order_book::c` (`find_package` component `c`) and described by
    `limit_order_book_c.pc`.

**Fixes:**

-   Made `cancel` ignore unknown order IDs and `get` return `NULL` for them
    in the C API. Both previously threw `std::out_of_range` across the C
    boundary, which aborted the calling C program or Python process.
-   Fixed the Python `ctypes` declaration of `Volume` to match the C++
    `uint64_t` width. The `volume`, `volume_sell`, and `volume_buy` methods
    previously truncated volumes above $2^{32} - 1$.

**Build and packaging:**

-   Replaced Travis CI with GitHub Actions workflows that run the Catch2
    suites on Linux and the Python suite and distribution builds on Linux,
    macOS, and Windows, and publish to PyPI through a trusted publisher.
-   Moved package metadata and the extension declaration into
    `pyproject.toml` and removed `setup.py`.
-   Removed `-march=native` and other host-tuned flags from the Python
    extension so wheels run on any CPU of their architecture. The extension
    now uses the compiler's default C++ standard; library code still compiles
    as C++14 for MSVC.
-   Moved every build output under the ignored `.build/` directory and
    excluded the tests from the wheel.
-   Removed the Windows-only `PyInit_lib_limit_order_book` stub, which
    setuptools needed to link the library as an extension; CMake builds a
    plain shared library.
-   Replaced SCons, the makefile, `MANIFEST.in`, and setuptools with a CMake
    build. The Python package builds through scikit-build-core, and
    [main.sh](main.sh) wraps the development commands.
-   Added a header-only `limit_order_book::limit_order_book` CMake target for
    FetchContent and `add_subdirectory`, and `cmake --install` rules for the
    headers, a `find_package` config package, and a pkg-config file. As a
    subproject, the library builds no tests or shared library.
-   Made the `project()` version in `CMakeLists.txt` the version of both the
    C++ and Python packages.
-   Replaced the vendored Catch2 with Catch2 v3.8.1 fetched by CMake, built
    every first-party target as C++14, and combined the Catch2 suites into one
    `ctest`-discovered binary. CI now runs the C++ suites and the package
    checks on Linux, macOS, and Windows.

**Documentation:**

-   Added `AGENTS.md`, the C++ and Markdown style guides, `CONTRIBUTING.md`,
    and `LICENSING.md`, and shipped `LICENSING.md` in both distributions.
-   Added `CITATION.bib`, `CITATION.cff`, and a README citation section.
-   Removed the `Doxyfile`, the Doxygen theme, and the generated `docs/` site,
    and retired the GitHub Pages documentation URL. The header comments are
    the API reference.

## 2.0.0 (2020-07-07)

**Breaking changes:**

-   Renamed the `Size` type to `Quantity` and the `size` arguments of the
    order methods to `quantity` in C++, C, and Python.
-   Renamed the order-count queries from `size_at`, `size_sell`, `size_buy`,
    and `size` to `count_at`, `count_sell`, `count_buy`, and `count` in C++,
    C, and Python, returning the new `Count` type.
-   Removed the `arrival` and `execution` timestamps from `Order` and the
    `arrival` arguments from the C++ order methods.
-   Widened the C++ `Volume` type from 32 to 64 bits.
-   Removed `get` from the Python class.

**Features:**

-   Added `reduce` to the C++ API to partially cancel a resting order.
-   Added the C++ `price`, `last_best_sell`, `last_best_buy`, `last_best`,
    and `last_price` queries, and `volume_sell_best` and `volume_buy_best`
    for the volume at the best limits. These are not yet exposed through the
    C or Python APIs.

**Internal:**

-   Moved the C API into `include/lib_lob.hpp` so `src/lib_lob.cpp` only
    compiles it, and moved the Python `ctypes` declarations into
    `limit_order_book/library.py`.
-   Added the Doxygen theme and generated the API reference into `docs/` for
    GitHub Pages.
-   Compiled each C++ test file into an independent binary, and added the
    MIT copyright header to every C++ source.

## 1.1.5 (2020-02-21)

-   Fixed `market_sell` and `market_buy` to construct their temporary `Order`
    directly, so they compile under C++14 now that `Order` cannot be copied.

## 1.1.4 (2020-02-21)

-   Passed the book by reference instead of by value in the benchmarks.

## 1.1.3 (2020-02-19)

-   Disabled copying of `Order` and `Limit` objects, and constructed orders in
    place with `std::forward`.
-   Not uploaded to PyPI; 1.1.4 and 1.1.5 include these changes.

## 1.1.2 (2020-02-17)

-   Added missing test cases and documentation.

## 1.1.1 (2020-02-16)

-   Fixed matching of incoming limit orders that cross the spread: a match
    that fills the incoming order now zeroes its remaining quantity, and a
    limit order that fills entirely no longer rests on the book.
-   Added regression tests for these fixes.

## 1.1.0 (2020-01-25)

-   Added `clear` to remove every order from a `LimitOrderBook`, with a
    Python binding.

## 1.0.1 (2019-12-17)

-   Added the vendored dependencies to `MANIFEST.in` so the source
    distribution builds.

## 1.0.0 (2019-12-17)

-   Released the price-time priority limit order book with C++, C, and Python
    `ctypes` APIs.

[releases]: https://github.com/Kautenja/limit-order-book/releases
