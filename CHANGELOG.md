# Changelog

Notable changes to `limit-order-book` are collected here. Entries before
2.0.0 come from the [GitHub Releases][releases] notes; all entries are checked
against the tagged sources and commit history. Dates through 2.0.0 are the
PyPI upload dates, or the tag date for releases that were not uploaded.

## 3.1.0 (2026-10-19)

**Breaking changes:**

-   Moved the adopted tree and list helpers from the global `BST` and `DLL`
    namespaces into `LOB::BST` and `LOB::DLL`, without global compatibility
    aliases. Direct C++ helper users must update their qualifiers; header
    filenames, node layouts, and the book, C, and Python APIs are unchanged.
    Replaced the standalone header guards with `LOB_BINARY_SEARCH_TREE_HPP_`
    and `LOB_DOUBLY_LINKED_LIST_HPP_`.

## 3.0.0 (2026-10-18)

**Breaking changes:**

-   Removed the low-level `limit_order_book.library` ctypes loader. Use the
    public `LimitOrderBook` class; its imports, methods, and integer wrapping
    are preserved. Invalid integer arguments now raise `TypeError` instead
    of `ctypes.ArgumentError`.
-   Raised the minimum Python version to 3.13. Added wheel builds for CPython
    3.13 and 3.14 on Linux x64 and arm64, macOS x64 and arm64, and Windows x64.
-   Replaced the C++-only `include/lib_lob.hpp` with the C99 header
    `include/limit_order_book_c.h`, which declares the C API on an opaque
    `LOB_LimitOrderBook` handle and `LOB_` fixed-width aliases.
    Renamed `src/lib_lob.cpp` to `src/limit_order_book_c.cpp` and moved the
    definitions there, so the header can be included from multiple translation
    units. Consumers must include `limit_order_book_c.h` and link the C library;
    C++ callers must use the opaque handle from `new_` in place of a
    `LOB::LimitOrderBook*`. The C function names are preserved.

**Features:**

-   Replaced Python ctypes calls with a Cython extension that calls C++
    directly. Wheels need neither Cython nor a separate C API shared library
    at runtime. Source builds use CMake 3.21 or later, a C++14 compiler, Python
    development headers, and Cython 3 installed as an isolated build dependency.
-   Added `./main.sh benchmark-python` for repeatable public Python API
    measurements and before/after comparisons.
-   Added the `LOB_BUILD_C_LIBRARY` CMake option, which builds and installs
    the C API as the versioned `liblimit_order_book` shared library, exported
    as `limit_order_book::c` (`find_package` component `c`) and described by
    `limit_order_book_c.pc`.

**Fixes:**

-   Ignored duplicate live limit-order IDs before matching or inserting into
    a price queue in C++, C, and Python. Duplicate submissions now preserve
    the resting order, FIFO links, and aggregates, even when they specify a
    different side or crossing price. IDs remain reusable after filling,
    cancellation, or clearing.
-   Gave every C++ benchmark iteration an independent empty book, with
    construction and cleanup outside timing. Repeated iterations previously
    reused live order IDs in the insertion and cancellation benchmarks,
    measuring ignored duplicates instead of the intended workload. Earlier
    results from benchmarks that reused book state are not directly comparable.
-   Made `cancel` ignore unknown order IDs and `get` return `NULL` for them
    in the C API. Both previously threw `std::out_of_range` across the C
    boundary, which aborted the calling C program or Python process.
-   Preserved the full C++ `uint64_t` volume in Python. The `volume`,
    `volume_sell`, and `volume_buy` methods previously truncated volumes above
    $2^{32} - 1$ through the ctypes binding; the Cython declarations now match
    the C++ widths, including on Windows.
-   Made Python object cleanup clear remaining price levels before deleting
    the owned C++ book, including when subclass initialization fails.

**Build and packaging:**

-   Pruned the unused nested Catch2 copies from `binary-search-tree` and
    `doubly-linked-list`. First-party tests and benchmarks continue to use
    Catch2 v3 fetched by CMake; standalone vendored test harnesses require
    the complete upstream repositories.
-   Included the retained vendored libraries' license files in wheel metadata.
-   Removed the unused `cpptqdm`, `hopscotch_map`, and `sparse-map` vendored
    libraries, obsolete packaging exclusions, unused benchmark includes,
    commented-out Python code, and dangling diagram references.
-   Removed the dummy matching executable and its build wiring. The
    `LOB_BUILD_EXAMPLES` option remains accepted as a deprecated no-op,
    with its default unchanged at `OFF`.
-   Replaced Travis CI with GitHub Actions workflows that run the Catch2
    suites, installed-package and FetchContent checks, Python suites, and
    distribution builds on Linux, macOS, and Windows. Added installed-wheel
    tests and PyPI publishing through a trusted publisher.
-   Moved package metadata into `pyproject.toml`, the extension build into
    `CMakeLists.txt`, and removed `setup.py`.
-   Removed `-march=native` and other host-tuned flags from the Python
    extension and built every first-party target as C++14, including on MSVC.
-   Moved native and Python build outputs under the root `.build/` directory
    and report outputs under `docs/whitepaper/.build/`; both are ignored by
    Git and removed by `./main.sh clean`. Excluded the tests from the wheel.
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
    C++ and Python packages. Set the release to 3.0.0, with C library
    `SOVERSION` 3, and updated the CMake examples and consumer checks to
    request major version 3.
-   Replaced the vendored Catch2 with Catch2 v3.8.1 fetched by CMake, built
    the tests and benchmarks against it, and combined the Catch2 suites into
    one `ctest`-discovered binary.
-   Added a reusable technical-report workflow that builds the PDF from
    archived evidence. Tag releases wait for package checks and the report
    build, then attach the distributions and `limit-order-book.pdf`; manually
    published releases also build and attach their tag's report.

**Documentation:**

-   Added the [technical report](docs/whitepaper/README.md) on matching
    semantics, architecture, complexity, ownership limits, and the language
    interfaces. Included an independent reference-model checker, repeatable
    study harness, raw measurements, source hashes, and reproduction commands.
-   Added `AGENTS.md`, the C++ and Markdown style guides, `CONTRIBUTING.md`,
    and `LICENSING.md`, and shipped `LICENSING.md` in both distributions.
-   Added `CITATION.bib`, `CITATION.cff`, and a README citation section.
-   Unified the technical report and software under the existing top-level
    2019 citation. The report now reads that BibTeX entry directly and shares
    its title; the separate report citation was removed.
-   Removed the `Doxyfile`, the Doxygen theme, and the generated `docs/` site,
    and retired the GitHub Pages documentation URL. The header comments are
    the API reference; the [documentation index](docs/README.md) links to
    development guides and attributed research references.

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
