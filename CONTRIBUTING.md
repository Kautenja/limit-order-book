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
headers, the C API, tests, benchmarks, and executables, and the
[Markdown Style Guide][markdown-style] for documentation. Match the
surrounding Python conventions in `limit_order_book/`. Keep formatting changes
focused on the code being changed, and preserve the MIT notice at the top of
each C++ file.

## Set Up Your Environment

| Work                       | Required tools                                         |
| -------------------------- | ------------------------------------------------------ |
| C++ tests and benchmarks   | Git, `g++` with C++17 support, and SCons               |
| Python package and library | Git, CPython 3.13 or 3.14, and a C++14 compiler        |
| Markdown documentation     | A text editor and Git; no native build required        |

SCons always compiles with `g++`. On macOS, `g++` is Apple Clang unless a GNU
compiler is installed and found first on `PATH`. The
[CI workflow](.github/workflows/ci.yml) runs the Catch2 suites with GCC on
Linux x64, and the Python suite with the platform compilers on Linux, macOS,
and Windows. Windows builds need the Visual Studio C++ build tools; see
[Usage Caveats](README.md#usage-caveats).

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

Create a virtual environment using a supported interpreter. On macOS or Linux:

```shell
python3.13 -m venv .venv
source .venv/bin/activate
```

On Windows, use PowerShell:

```powershell
py -3.13 -m venv .venv
.venv\Scripts\Activate.ps1
```

Python 3.14 can be used instead. With the environment active:

```shell
python -m pip install --upgrade pip
python -m pip install -r requirements.txt
python -m pip install --editable .
python -c "from limit_order_book import LimitOrderBook; LimitOrderBook()"
```

The editable install compiles the C API into
`limit_order_book/lib_limit_order_book*` so tests run against the checkout.
Repeat the editable install, or run `scons limit_order_book`, after changing
C++ sources; editing them alone does not rebuild the shared library.
[pyproject.toml](pyproject.toml) declares the package and its extension;
[requirements.txt](requirements.txt) contains development tools.

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
-   **[include/lib_lob.hpp](include/lib_lob.hpp) and
    [src/lib_lob.cpp](src/lib_lob.cpp):** The `extern "C"` API and the
    translation unit that compiles it into the shared library.
-   **[limit_order_book/library.py](limit_order_book/library.py) and
    [limit_order_book/limit_order_book.py](limit_order_book/limit_order_book.py):**
    The `ctypes` declarations and the Python class that wraps them.
-   **[test](test), [limit_order_book/test](limit_order_book/test),
    [benchmark](benchmark), and [main](main):** Catch2 suites, the Python
    suite, Catch2 benchmarks, and small executables.
-   **[SConstruct](SConstruct), [makefile](makefile), and
    [pyproject.toml](pyproject.toml):** The C++ build, the combined test
    target, and the Python package configuration.

### Compatibility And Invariants

The C++, C, and Python interfaces mirror each other. When the C API changes,
update `lib_lob.hpp`, the `argtypes` and `restype` declarations in
`library.py`, the Python wrapper, and both test suites in the same change.
`ctypes` types must match the C++ type widths exactly. Renaming or removing a
C++ method, C function, or Python method is a breaking change for users who
copy `include/` or install from PyPI; discuss it in an issue first.

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

SCons compiles with `-std=c++17`, but the Python extension uses the compiler
default, which is C++14 for MSVC. Code in `include/` and `src/` must compile
as C++14 for the Windows wheels. Wheels must stay portable, so never add
`-march=native` or other host-tuned flags to `pyproject.toml`.

Do not edit `vendor/`. Fixes to the owner-authored `binary-search-tree`,
`doubly-linked-list`, and `cpptqdm` libraries belong in their upstream
repositories. Do not regenerate `docs/` in a pull request; it is the published
Doxygen site.

## Development And Testing

### All Suites

Run every suite with:

```shell
make test
```

That target runs the Catch2 suites, builds the shared library and copies it
into `limit_order_book/`, and runs the Python suite.

### C++ Tests

Build and run every Catch2 suite, or one suite:

```shell
scons test
scons test/test_limit_tree.cpp
```

Reproduce a bug with a failing test before fixing it when practical, and test
observable book state rather than mirroring implementation details.

### Python Tests

After building the shared library with `scons` or the editable install, run:

```shell
python -m unittest discover .
```

### Benchmarks

Build and run every benchmark, or one benchmark:

```shell
scons benchmark
scons benchmark/benchmark_lob.cpp
```

SCons builds benchmarks with `-march=native`, so results describe the local
host only. Compare matched before and after workloads with the same compiler
and flags, repeated runs, and an effect larger than the run-to-run noise.
Record the host, architecture, and compiler version with results. A passing
build does not establish a speedup.

### Distribution Builds

Build a source distribution and a wheel with:

```shell
python -m build --outdir .build/dist
```

All build outputs go to the ignored `.build/` directory. To remove them and
the shared library copied into the package, run:

```shell
make clean
```

The CI workflow runs on pull requests, pushes to `master`, and tag pushes. It
covers the Catch2 suites on Linux x64, and the Python suite and distribution
builds for CPython 3.13 and 3.14 on Linux x64 and arm64, macOS x64 and arm64,
and Windows x64. The benchmarks are not run in CI; include local results for
changes that need them.

### Choosing Validation

| Change                       | Relevant validation                                         |
| ---------------------------- | ----------------------------------------------------------- |
| Markdown guidance            | Check links, paths, and commands; run `git diff --check`    |
| Matching or data structures  | Run `make test`; benchmark before and after for hot paths   |
| C API or Python bindings     | Run `make test`; check every C function's `ctypes` types    |
| Build or packaging           | Run `make test` and build both distributions                |
| Windows-sensitive C++        | Confirm the code compiles as C++14; rely on CI for MSVC     |

Do not claim Linux or Windows validation from a local macOS build.

## Prepare A Release

Update `version` in [pyproject.toml](pyproject.toml), move the
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
[cpp-style]: developer-manual/style-guide-cpp.md
[markdown-style]: developer-manual/style-guide-markdown.md
