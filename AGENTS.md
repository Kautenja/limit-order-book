# Limit Order Book Agent Instructions

`limit-order-book` is a header-only C++ limit order book with a price-time
priority matching policy, distributed primarily as a CMake package. A C API is
compiled into a shared library, and a Python package binds it through
`ctypes`. Preserve matching correctness, the complexity guarantees of the book
operations, the CMake package interface, and parity between the C++, C, and
Python interfaces.

## Start Here

Read this file and the sources relevant to the requested change:

-   [README.md](README.md): public overview, installation, and the official
    test and benchmark commands.
-   [C++ Style Guide](docs/development/style-guide-cpp.md): required for
    headers, the C API, tests, benchmarks, and executables.
-   [Markdown Style Guide](docs/development/style-guide-markdown.md): required
    for `README.md`, this file, and first-party documentation under `docs/`.
-   [LICENSE](LICENSE): MIT terms for first-party code.
-   [Limit order book research](docs/references/lob.md): imported background
    on fast limit order book design. It is quoted reference material, not
    project guidance.

The style guides adapt the conventions from Christian Kauten's other projects
to this repository. They are self-contained; the other projects are reference
material, not dependencies.

## Repository Layout

-   **`include/`:** The C++ library. `structures.hpp` defines `Order`, `Limit`,
    and the domain types; `limit_tree.hpp` defines one side of the book;
    `limit_order_book.hpp` combines both sides; `lib_lob.h` is the C API.
-   **`src/lib_lob.cpp`:** The definitions of the C API, compiled into the
    shared library that C programs (`LOB_BUILD_C_LIBRARY=ON`) and the Python
    package load.
-   **`limit_order_book/`:** The Python package. `library.py` declares the
    `ctypes` signatures; `limit_order_book.py` wraps them in a class.
-   **`test/` and `limit_order_book/test/`:** Catch2 and `unittest` suites.
-   **`benchmark/`:** Catch2 benchmarks for the C++ book.
-   **`main/`:** Small executables that drive the book, built with
    `LOB_BUILD_EXAMPLES=ON`.
-   **`CMakeLists.txt` and `cmake/`:** The C++ build, install rules, and the
    CMake package and pkg-config templates. The `project()` version is the
    version of both the C++ and Python packages.
-   **`test/package/`:** C++ and C consumer projects that check
    `find_package` and FetchContent.
-   **`main.sh`:** The development command wrapper, modeled on nes-py's.
-   **`.build/`:** Every build output, ignored by Git: the CMake build in
    `.build/cmake/`, scikit-build-core builds in `.build/python/`, package
    checks in `.build/prefix/` and `.build/package-*/`, and distributions in
    `.build/dist/`. `./main.sh clean` removes it.
-   **`vendor/`:** Third-party and owner-authored dependencies, committed
    in-tree. Treat them as read-only.
-   **`docs/`:** Documentation, indexed in [docs/README.md](docs/README.md).
    `docs/development/` holds durable rules for agents and maintainers;
    `docs/references/` holds imported research notes and example engines.

## Working In This Repository

-   Inspect `git status --short` and relevant diffs before editing. Preserve
    existing user work.
-   Implement one coherent requested change at a time. Read the nearby code
    and tests first. Avoid unrelated reformatting, dependency upgrades, or
    new build infrastructure.
-   Do not edit `vendor/`. Fixes to the owner-authored `binary-search-tree`,
    `doubly-linked-list`, and `cpptqdm` libraries belong in their upstream
    repositories; ask before changing a vendored copy here.
-   Reserve `.build/` for build outputs. Put scratch files, experiments, and
    benchmark captures outside the repository, and remove them when the task
    is finished. Do not add new top-level build or output directories.
-   Commit only when requested. Push, tag, and publish to PyPI only when
    requested. Keep credentials and private local paths out of tracked files.

## Architecture And Contracts

The library is layered, and each layer must stay consistent with the one
below it:

1.  `structures.hpp`: `Order` is a doubly-linked-list node; `Limit` is a
    binary-search-tree node that owns a FIFO queue of orders at one price.
2.  `limit_tree.hpp`: `LimitTree<Side>` owns the limits for one side, a
    price-to-limit hash map, the best limit pointer, and aggregate counts.
3.  `limit_order_book.hpp`: `LimitOrderBook` owns both trees and the
    order-ID-to-order map, and crosses incoming limit orders against the
    opposite side.
4.  `lib_lob.h` and `src/lib_lob.cpp`: C functions on an opaque handle that
    mirror `LimitOrderBook`.
5.  `limit_order_book/`: `ctypes` declarations and the Python class.

Preserve these matching invariants:

-   Orders at one price execute in arrival order (price-time priority).
-   The best pointer of each tree is the highest buy or lowest sell limit, or
    `nullptr` when the side is empty.
-   A limit is removed from its tree and hash map as soon as its last order
    leaves. An order is erased from the order map as soon as it is filled or
    cancelled.
-   `Limit::count`, `Limit::volume`, and the tree aggregates always equal the
    sums over the orders they contain.
-   Adding an order at an existing limit, cancelling, and executing at the
    best limit stay $O(1)$; adding a new limit stays $O(\log M)$ in the number
    of limits.

Preserve the public interface. Renaming or removing a C++ method, C function,
or Python method is a breaking change for users who copy `include/` or
install from PyPI; call it out and get agreement first. When the C API
changes, update `lib_lob.h`, `src/lib_lob.cpp`, the `argtypes` and `restype`
declarations in `library.py`, the Python wrapper, the Python suite, and the C
consumer in `test/package/` in the same change. `ctypes` types must match the
C++ type widths exactly.

Downstream C++ projects depend on the CMake interface, so treat it as public:
the `limit_order_book::limit_order_book` and `limit_order_book::c` targets,
the `LOB_*` options and their defaults, the install layout under
`include/limit_order_book/`, and the package and pkg-config names. By
default, a FetchContent or `add_subdirectory` consumer must get only the
header-only target: no tests, Catch2 download, or shared library; it gets
`limit_order_book::c` only with `LOB_BUILD_C_LIBRARY=ON`.
`./main.sh test-package` checks both consumer paths from C++ and C.

## Development And Validation

CMake 3.21 or later builds the C++ code, and scikit-build-core drives the
same `CMakeLists.txt` for the Python wheel. [main.sh](main.sh) wraps the
common commands; run them from the repository root:

```shell
./main.sh test
```

That command configures `.build/cmake/`, builds the Catch2 tests, benchmarks,
examples, and the C API library (copied into `limit_order_book/`), runs the
Catch2 suite through `ctest`, and runs the Python suite. Other commands:

-   **`./main.sh test-cpp`:** Build and run the Catch2 suite. Extra arguments
    go to `ctest`, such as `-R "send single order"`.
-   **`./main.sh test-python`:** Build the C API library and run the Python
    suite.
-   **`./main.sh test-package`:** Install the headers and the C API library
    into `.build/prefix/` and build `test/package/` with `find_package` and
    with FetchContent of the checkout.
-   **`./main.sh benchmark`:** Build and run the benchmarks. Extra arguments
    go to the Catch2 runner.
-   **`./main.sh deployment`:** Build the sdist and wheel in `.build/dist/`.

The Catch2 v3 dependency is fetched from GitHub by CMake on first configure,
so the C++ tests need network access once.

**Important:** CMake builds every first-party target as C++14, which is also
the MSVC default used for the Windows wheels. Keep `include/` and `src/`
C++14-clean. Wheels and installed packages must stay portable, so never add
`-march=native` or other host-tuned flags to `CMakeLists.txt`.

Validate changes by type:

-   **Matching or data structures:** `./main.sh test`, plus a benchmark run
    before and after the change when the hot path is touched.
-   **C API or Python bindings:** `./main.sh test`, and confirm every C
    function has matching `ctypes` declarations.
-   **Build or packaging:** `./main.sh test`, `./main.sh test-package`, and
    `./main.sh deployment`. Do not run `./main.sh ship`, dispatch the publish
    workflow, or upload to PyPI unless asked.
-   **Documentation only:** Link, path, and command checks, `git diff --check`,
    and a review of the rendered Markdown. A C++ build is not required.

Performance claims need comparable workloads, the same compiler flags, repeated
runs, and an effect larger than the run-to-run noise. A passing build is not
evidence of a speedup. Report missing prerequisites and checks that could not
run. Do not claim Linux or Windows validation from a local macOS build.

## Planning And Completion

Small changes can be planned in chat. For substantial work that needs a
durable plan, create `specs/NNN-feature-name.md` with a creation date, status,
goal, scope, non-goals, testable acceptance criteria, and exact validation
commands. Use the next unused three-digit number. Record completion evidence
in the owning spec: date, decisions, commands and results, and limitations.
Mark verified work `Status: COMPLETE`. Do not create separate history logs.

When a change creates a durable rule for future work, record it in the
owning guide under `docs/development/` rather than in a spec or this file.

Finish with a concise summary of the changes, the validation actually run,
and any failures or skipped checks. Include the commit and push result when
those actions were requested.
