# Limit Order Book Agent Instructions

`limit-order-book` is a header-only C++ limit order book with a price-time
priority matching policy, a C API compiled into a shared library, and a Python
package that binds the C API through `ctypes`. Preserve matching correctness,
the complexity guarantees of the book operations, and parity between the C++,
C, and Python interfaces.

## Start Here

Read this file and the sources relevant to the requested change:

-   [README.md](README.md): public overview, installation, and the official
    test and benchmark commands.
-   [C++ Style Guide](developer-manual/style-guide-cpp.md): required for
    headers, the C API, tests, benchmarks, and executables.
-   [Markdown Style Guide](developer-manual/style-guide-markdown.md): required
    for `README.md`, this file, the developer manual, and notes.
-   [LICENSE](LICENSE): MIT terms for first-party code.
-   [notes/lob.md](notes/lob.md): imported background on fast limit order book
    design. It is quoted reference material, not project guidance.

The style guides adapt the conventions from Christian Kauten's other projects
to this repository. They are self-contained; the other projects are reference
material, not dependencies.

## Repository Layout

-   **`include/`:** The C++ library. `structures.hpp` defines `Order`, `Limit`,
    and the domain types; `limit_tree.hpp` defines one side of the book;
    `limit_order_book.hpp` combines both sides; `lib_lob.hpp` is the C API.
-   **`src/lib_lob.cpp`:** The translation unit that compiles the C API into
    the shared library.
-   **`limit_order_book/`:** The Python package. `library.py` declares the
    `ctypes` signatures; `limit_order_book.py` wraps them in a class.
-   **`test/` and `limit_order_book/test/`:** Catch2 and `unittest` suites.
-   **`benchmark/`:** Catch2 benchmarks for the C++ book.
-   **`main/`:** Small executables that drive the book, built by SCons.
-   **`vendor/`:** Third-party and owner-authored dependencies, committed
    in-tree. Treat them as read-only.
-   **`developer-manual/`:** Durable rules for agents and maintainers.
-   **`doxygen/`:** Doxygen header, footer, stylesheet, and images.
-   **`docs/`:** Generated Doxygen HTML served by GitHub Pages from `master`.
    Never hand-edit it or put Markdown there.
-   **`notes/`:** Imported research notes and example engines.

## Working In This Repository

-   Inspect `git status --short` and relevant diffs before editing. Preserve
    existing user work.
-   Implement one coherent requested change at a time. Read the nearby code
    and tests first. Avoid unrelated reformatting, dependency upgrades, or
    new build infrastructure.
-   Do not edit `vendor/`. Fixes to the owner-authored `binary-search-tree`,
    `doubly-linked-list`, and `cpptqdm` libraries belong in their upstream
    repositories; ask before changing a vendored copy here.
-   Do not regenerate `docs/` unless asked. A regenerated `docs/` changes the
    public site once it reaches `master`.
-   Put scratch files, build experiments, and benchmark captures outside the
    repository or in an ignored `build*/` directory. Remove them when the task
    is finished.
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
4.  `lib_lob.hpp`: `extern "C"` functions that mirror `LimitOrderBook`.
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
changes, update `lib_lob.hpp`, the `argtypes` and `restype` declarations in
`library.py`, the Python wrapper, and both test suites in the same change.
`ctypes` types must match the C++ type widths exactly.

## Development And Validation

The C++ build uses SCons with the system `g++`. The Python package uses
`pyproject.toml` for metadata and `setup.py` for the extension. From the
repository root:

```shell
make test
```

That target runs the Catch2 suites, builds the shared library into
`limit_order_book/`, and runs the Python suite. The individual stages are:

```shell
scons test
scons
python3 -m unittest discover .
```

Run one Catch2 suite with its alias, such as `scons test/test_limit_tree.cpp`,
and the benchmarks with `scons benchmark`.

**Important:** SCons compiles with `-std=c++17`, but `setup.py` compiles the
PyPI extension with `-std=c++1y`, or `/std:c++14` under MSVC. Code in
`include/` and `src/` must compile under all three until `setup.py` is
updated. Wheels must stay portable, so never add `-march=native` or other
host-tuned flags to `setup.py`.

**Note:** The vendored Catch2 2.9 header uses an x86-only debugger trap, so
the Catch2 suites and benchmarks fail to compile on Apple Silicon. On arm64
macOS, `scons test` and `make test` fail before any test runs; build the
shared library into the package with `scons limit_order_book` and run the
Python suite.
Report the C++ suites as not run rather than passed.

Validate changes by type:

-   **Matching or data structures:** `make test`, plus a benchmark run before
    and after the change when the hot path is touched.
-   **C API or Python bindings:** `make test`, and confirm every C function has
    matching `ctypes` declarations.
-   **Build or packaging:** `make test` and `python3 -m build`. Do not run
    `make ship`, dispatch the publish workflow, or upload to PyPI unless
    asked.
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
owning guide under `developer-manual/` rather than in a spec or this file.

Finish with a concise summary of the changes, the validation actually run,
and any failures or skipped checks. Include the commit and push result when
those actions were requested.
