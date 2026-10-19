# Contributing To limit-order-book

Help improve `limit-order-book` through bug reports, documentation, tests, and
code. This guide takes you from a fresh checkout to a tested contribution and
explains the architecture and compatibility requirements used in review.

-   [Set up your environment](#set-up-your-environment)
-   [Understand the architecture](#architecture)
-   [Build and test](#development-and-testing)
-   [Choose validation for your change](#choosing-validation)
-   [Prepare a release](#prepare-a-release)
-   [Submit a pull request](#submit-a-pull-request)

## Before You Start

Search the [existing issues][issues] before reporting a bug or proposing a
feature. Include the operating system, CPU architecture, compiler version,
Python and `limit-order-book` versions, and install method when relevant.
Provide a minimal reproducer: the sequence of limit, market, and cancel calls
with their order IDs, quantities, and prices, and the expected versus observed
book state, such as the best prices, volumes, and counts. Include the
complete relevant error output. Discuss substantial API or matching behavior
changes in an issue first.

Read the [README](README.md), the [changelog](CHANGELOG.md), and the
[licensing guide](LICENSING.md). Follow the [C++ Style Guide][cpp-style] for
headers, the C API, tests, and benchmarks, and the
[Markdown Style Guide][markdown-style] for documentation. Match the
surrounding Python conventions in `limit_order_book/`. Keep formatting changes
focused on the code being changed, and preserve the MIT notice at the top of
each C++ file.

## Set Up Your Environment

| Work                       | Required tools                                         |
| -------------------------- | ------------------------------------------------------ |
| C++ tests and benchmarks   | Git, CMake 3.21 or later, and a C++14 compiler         |
| Python package and library | The C++ tools above and CPython 3.13 or 3.14           |
| Markdown documentation     | A text editor and Git; no native build required        |

CMake uses the platform's default compiler and fetches Catch2 with Git the
first time the tests are configured. [main.sh](main.sh) is a Bash script; on
Windows, run it from Git Bash. The [CI workflow](.github/workflows/ci.yml) runs
the C++ suites, the package checks, and the Python suite with the platform
compilers on Linux, macOS, and Windows. Windows builds need the Visual Studio
C++ build tools; see [Usage Caveats](README.md#usage-caveats).

### Get The Source

Fork the repository on GitHub, then substitute your fork's clone URL below:

```shell
git clone https://github.com/Kautenja/limit-order-book.git
cd limit-order-book
git switch -c docs/contributor-setup
```

Choose a branch name describing your change. The dependencies in `vendor/` are
committed in-tree, so no submodule checkout is needed. Run the remaining
commands from the repository root unless stated otherwise.

### Install For Development

Create `.venv` with the development tools from
[requirements.txt](requirements.txt):

```shell
./main.sh install
```

`main.sh` uses `.venv` automatically when it exists; set `PYTHON` to choose
another interpreter, such as `PYTHON=python3.14 ./main.sh install`. Then
build everything and run both suites:

```shell
./main.sh test
```

The CMake build copies the Cython extension into `limit_order_book/_native*`
so the Python suite runs against the checkout. Rebuild with
`./main.sh test-python` after changing C++ headers or Cython sources. See
[Python bindings](docs/development/python-bindings.md) for ownership,
conversion, and build requirements.

## Architecture

### Source Map

-   **[include/structures.hpp](include/structures.hpp):** `Order`, a
    doubly-linked-list node, `Limit`, a binary-search-tree node that owns the
    FIFO queue of orders at one price, and the domain types.
-   **[include/limit_tree.hpp](include/limit_tree.hpp):** `LimitTree<Side>`,
    one side of the book with its limits, price-to-limit hash map, best limit,
    and aggregate counts.
-   **[include/limit_order_book.hpp](include/limit_order_book.hpp):**
    `LimitOrderBook`, which owns both trees and the order map and crosses
    incoming orders against the opposite side.
-   **[include/limit_order_book_c.h](include/limit_order_book_c.h) and
    [src/limit_order_book_c.cpp](src/limit_order_book_c.cpp):** The C API
    header, with declarations only, and the translation unit that defines it
    in the shared library.
-   **[limit_order_book/_book.pxd](limit_order_book/_book.pxd) and
    [limit_order_book/_native.pyx](limit_order_book/_native.pyx):** Cython
    declarations and methods that call the C++ book directly.
    `limit_order_book.py` preserves the public Python class.
-   **[test](test), [limit_order_book/test](limit_order_book/test),
    and [benchmark](benchmark):** Catch2 suites, the Python suite, and Catch2
    benchmarks, plus the Python interface benchmark.
-   **[CMakeLists.txt](CMakeLists.txt) and [cmake](cmake):** The C++ build,
    install rules, and the CMake package and pkg-config templates. The
    `project()` version is the version of both packages.
-   **[test/package](test/package):** A consumer project that checks the
    installed package and FetchContent from C++ and from C.
-   **[pyproject.toml](pyproject.toml) and [main.sh](main.sh):** The Python
    package configuration and the development command wrapper.

### Compatibility And Invariants

The C and Python interfaces independently wrap the C++ book. When the C API
changes, update `limit_order_book_c.h`, `src/limit_order_book_c.cpp`, and the
C consumer tests together. Keep equivalent Python operations and tests
consistent. Cython declarations must match the C++ type widths exactly.
Renaming or removing a C++ method, C function, or Python method is a breaking
change for users who copy `include/` or install from PyPI; discuss it in an
issue first.

Matching changes must preserve these invariants:

-   Orders at one price execute in arrival order (price-time priority).
-   The best pointer of each tree is the highest buy or lowest sell limit, or
    `nullptr` when the side is empty.
-   A limit leaves its tree and hash map as soon as its last order leaves, and
    an order leaves the order map as soon as it is filled or cancelled.
-   `Limit::count`, `Limit::volume`, and the tree aggregates equal the sums
    over the orders they contain.
-   Adding an order at an existing limit, cancelling, and executing at the
    best limit stay $O(1)$; adding a new limit stays $O(\log M)$ in the number
    of limits.

The headers require C++14, and CMake builds every first-party target as C++14
to keep them that way for consumers and the MSVC wheels. Wheels and installed
packages must stay portable, so never add `-march=native` or other host-tuned
flags to `CMakeLists.txt`. Keep `limit_order_book::limit_order_book`, the
`LOB_*` options, and the install layout stable; downstream projects depend on
them.

Do not edit `vendor/`. Fixes to the owner-authored `binary-search-tree`,
`doubly-linked-list`, and `cpptqdm` libraries belong in their upstream
repositories.

## Development And Testing

### All Suites

Run every suite with:

```shell
./main.sh test
```

That command configures `.build/cmake/`, builds the tests, benchmarks, and
Cython extension, runs the Catch2 suite with `ctest`, and runs the Python suite.

### C++ Tests

Build and run the Catch2 suite, or one test case through `ctest`:

```shell
./main.sh test-cpp
./main.sh test-cpp -R "send single order"
```

Check that downstream CMake projects can consume the library, both installed
and through FetchContent:

```shell
./main.sh test-package
```

Reproduce a bug with a failing test before fixing it when practical, and test
observable book state rather than mirroring implementation details.

### Python Tests

Build the Cython extension into the package and run the Python suite:

```shell
./main.sh test-python
```

### Benchmarks

Build and run every benchmark, or one benchmark by name:

```shell
./main.sh benchmark
./main.sh benchmark "Spam new Limits"
```

Measure the public Python interface with repeated samples:

```shell
./main.sh benchmark-python --loops 100000 --repeats 9 --output /tmp/lob-python.json
```

The benchmarks build in the `Release` configuration without host-tuned flags.
Compare matched before and after workloads with the same compiler
and flags, repeated runs, and an effect larger than the run-to-run noise.
Record the host, architecture, and compiler version with results. A passing
build does not establish a speedup.

### Distribution Builds

Build a source distribution and a wheel in `.build/dist/` with:

```shell
./main.sh deployment
```

scikit-build-core drives the CMake build for the wheel. All build outputs go
to the ignored `.build/` directory. To remove them and the Cython extension
copied into the package, run:

```shell
./main.sh clean
```

The CI workflow runs on pull requests, pushes to `master`, and tag pushes. It
covers the C++ suites and package checks, and the Python suite and
distribution builds for CPython 3.13 and 3.14, on Linux x64 and arm64, macOS
x64 and arm64, and Windows x64. The benchmarks are not run in CI; include
local results for changes that need them.

### Choosing Validation

| Change                      | Relevant validation                                             |
| --------------------------- | --------------------------------------------------------------- |
| Markdown guidance           | Check links, paths, and commands; run `git diff --check`        |
| Matching or data structures | Run `./main.sh test`; benchmark hot paths before and after      |
| C API or Python bindings    | Run `./main.sh test`; check binding widths and benchmark Python |
| Build or packaging          | Run `./main.sh test`, `test-package`, and `deployment`          |
| Windows-sensitive C++       | Confirm the code compiles as C++14; rely on CI for MSVC         |

Do not claim Linux or Windows validation from a local macOS build.

## Prepare A Release

Update `VERSION` in the `project()` call of [CMakeLists.txt](CMakeLists.txt),
move the
[changelog](CHANGELOG.md)'s unreleased notes under a new version heading, and
validate the intended release revision. Call out breaking changes to the C++,
C, or Python interfaces explicitly. Preserve the established 2019 citation in
[CITATION.bib](CITATION.bib), [CITATION.cff](CITATION.cff), and the README;
routine releases do not create a new bibliographic work.

Push a tag that matches the package version, optionally prefixed with `v`. Do
not move or reuse a published tag. Tag CI builds the distributions and creates
or updates a GitHub release with them. Publishing that release starts the
[Publish to PyPI](.github/workflows/publish.yml) workflow, which builds the
sdist and wheels and uploads them through the `pypi` environment's trusted
publisher. A release created by the CI workflow's token does not trigger
another workflow, so if publishing does not start, dispatch it against the
release tag:

```shell
gh workflow run publish.yml --ref <tag>
```

Confirm both the workflow result and the package's availability on PyPI
before announcing the release.

## Submit A Pull Request

1.  Keep one coherent change per contribution. Update affected documentation
    and the changelog's unreleased section alongside behavior changes, and
    avoid unrelated formatting or dependency upgrades.
2.  Review the intended changes:

    ```shell
    git diff --check
    git diff
    git status --short
    ```

3.  Commit only the intended files, push your branch to your fork, and open a
    pull request against `master`. Describe the problem, the resulting
    behavior, any related issue, and the validation commands and results.
4.  Include the OS, CPU architecture, and compiler and Python versions where
    relevant. Distinguish builds, tests, and measurements, and state skipped
    checks and unresolved failures explicitly.

[issues]: https://github.com/Kautenja/limit-order-book/issues
[cpp-style]: docs/development/style-guide-cpp.md
[markdown-style]: docs/development/style-guide-markdown.md
