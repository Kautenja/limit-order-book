# Limit Order Book

[![CI status][build-badge]][build-server]
[![C++14][cpp-version]][build-config]
[![CMake 3.21+][cmake-version]][build-config]
[![PyPI version][pypi-version]][pypi-home]
[![Python 3.13+][python-version]][python-config]
[![Development status: beta][project-status]][python-config]
[![License: MIT][license-badge]][license]

[build-badge]: https://github.com/Kautenja/limit-order-book/actions/workflows/ci.yml/badge.svg
[build-server]: https://github.com/Kautenja/limit-order-book/actions/workflows/ci.yml
[cpp-version]: https://img.shields.io/badge/C%2B%2B-14-blue
[cmake-version]: https://img.shields.io/badge/CMake-3.21%2B-blue
[build-config]: CMakeLists.txt
[pypi-version]: https://img.shields.io/pypi/v/limit-order-book.svg
[pypi-home]: https://pypi.org/project/limit-order-book/
[python-version]: https://img.shields.io/badge/Python-3.13%2B-blue
[python-config]: pyproject.toml
[project-status]: https://img.shields.io/badge/status-beta-yellow
[license-badge]: https://img.shields.io/badge/license-MIT-blue
[license]: LICENSE

This is a header-only C++ implementation of the limit order book with a
price-time priority matching policy for reception of market data streams. The
C++ library is the primary product; a C API and a Python package with Cython
bindings to the C++ book are also provided.

The [technical report](docs/whitepaper/README.md) explains the design's prior
work, matching semantics, implementation limits, and reproducible evaluation.
Its [LaTeX source](docs/whitepaper/main.tex) and raw measurements are included.

## Usage

### C++

The library is header-only and requires C++14. Its binary search tree and
doubly linked list are maintained in [include/](include) as part of this
project, under the `LOB::BST` and `LOB::DLL` namespaces. See
[Data structures](docs/development/data-structures.md) for direct helper usage.
The remaining dependency, `robin-map` v1.3.0, is vendored as upstream headers
and a license in [vendor/](vendor) and installed with it. Its commit and
checksums are pinned; see [Vendored dependencies](docs/development/vendored-dependencies.md)
for verification and updates. Every method below provides the
`limit_order_book::limit_order_book` target, or the include path, so that this
works:

```cpp
#include "limit_order_book.hpp"

int main() {
    LOB::LimitOrderBook book;
    book.limit(LOB::Side::Sell, 1, 100, 50);
    book.limit(LOB::Side::Buy, 2, 40, 50);  // crosses and partially fills
    return book.best_sell() == 50 ? 0 : 1;
}
```

#### CMake FetchContent

Add the library to a CMake project without installing it. Pin `GIT_TAG` to a
release tag or commit:

```cmake
include(FetchContent)
FetchContent_Declare(
    limit_order_book
    GIT_REPOSITORY https://github.com/Kautenja/limit-order-book.git
    GIT_TAG master
    GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(limit_order_book)

target_link_libraries(my_app PRIVATE limit_order_book::limit_order_book)
```

As a subproject, the library builds no tests, benchmarks, or shared library.
`add_subdirectory` on a copy of this repository works the same way.

`LOB_BUILD_EXAMPLES` is deprecated and has no effect. It remains accepted
with a default of `OFF` for compatibility with existing CMake configurations.

#### System Installation

Install the headers, a CMake package, and a pkg-config file. This requires
CMake 3.21 or later and a C++14 compiler:

```shell
git clone https://github.com/Kautenja/limit-order-book.git
cd limit-order-book
cmake -S . -B .build/cmake -DLOB_BUILD_TESTS=OFF -DLOB_BUILD_PYTHON_LIBRARY=OFF
cmake --install .build/cmake --prefix /usr/local
```

Use any prefix, such as `"${HOME}/.local"`, and `sudo` when the prefix needs
it. The install puts every header in `include/limit_order_book/`, the CMake
package in `share/cmake/limit_order_book/`, the pkg-config file in
`share/pkgconfig/`, and the licenses in `share/doc/limit_order_book/`. Then
use the package from CMake:

```cmake
find_package(limit_order_book 3 CONFIG REQUIRED)
target_link_libraries(my_app PRIVATE limit_order_book::limit_order_book)
```

or from any build system through pkg-config:

```shell
c++ -std=c++14 $(pkg-config --cflags limit_order_book) my_app.cpp
```

#### Copying The Headers

Copy [include/](include) and [vendor/robin-map/include/](vendor/robin-map/include)
into your project, and add both to the include path.

### C

The C API in [include/limit_order_book_c.h](include/limit_order_book_c.h) is
a C99 header that wraps the book in an opaque `LOB_LimitOrderBook` handle.
Build and install it with `LOB_BUILD_C_LIBRARY=ON`. C consumers do not need
Python or Cython:

```shell
cmake -S . -B .build/cmake -DLOB_BUILD_TESTS=OFF \
    -DLOB_BUILD_PYTHON_LIBRARY=OFF -DLOB_BUILD_C_LIBRARY=ON
cmake --build .build/cmake
cmake --install .build/cmake --prefix /usr/local
```

The install adds `limit_order_book_c.h` to `include/limit_order_book/`, the
`liblimit_order_book` shared library to `lib/`, and a
`limit_order_book_c.pc` pkg-config file to `lib/pkgconfig/`. Because the
package now holds a compiled library, the CMake package moves to
`lib/cmake/limit_order_book/`. Link the `limit_order_book::c` target:

```cmake
find_package(limit_order_book 3 CONFIG REQUIRED COMPONENTS c)
target_link_libraries(my_app PRIVATE limit_order_book::c)
```

or use pkg-config:

```shell
cc $(pkg-config --cflags limit_order_book_c) my_app.c \
    $(pkg-config --libs limit_order_book_c)
```

A FetchContent or `add_subdirectory` consumer gets `limit_order_book::c` by
setting `LOB_BUILD_C_LIBRARY` to `ON` before adding the project.

### Python

The preferred Python installation of `limit-order-book` is from `pip`:

```shell
pip install limit-order-book
```

Wheels are published for CPython 3.13 and 3.14 on Linux (x64, arm64), macOS
(x64, arm64), and Windows (x64). Other platforms build from the source
distribution and need a C++14 compiler and Python development headers. The
source build installs Cython as an isolated build dependency; installed wheels
do not need Cython or a separate C API shared library.

The public `LimitOrderBook` class calls C++ through the compiled
`limit_order_book._native` extension. Existing imports and methods are
preserved:

```python
from limit_order_book import LimitOrderBook

book = LimitOrderBook()
book.limit_sell(order_id=1, quantity=100, price=50)
book.market_buy(order_id=2, quantity=40)
assert book.volume_sell(50) == 60
```

## Usage Caveats

### Windows

You'll need to install the Visual-Studio 17.0 tools for Windows installation.
The [Visual Studio Community](https://visualstudio.microsoft.com/downloads/)
package provides these tools for free.

## Development

See the [documentation index](docs/README.md) for contributor guidance,
style guides, and reference material.

[main.sh](main.sh) wraps the CMake and Python commands; run `./main.sh -h` for
the full list. Development needs CMake 3.21 or later, a C++14 compiler, Git
(CMake fetches Catch2 for the tests), and Python 3.13 or later. Set up a
virtual environment with the build tools:

```shell
./main.sh install
```

### Testing

To run the C++ and Python test suites, run:

```shell
./main.sh test
```

To run them separately, run `./main.sh test-cpp` or `./main.sh test-python`.
The C++ suite runs through `ctest`; the Python suite runs against the Cython
extension that the CMake build copies into `limit_order_book/`. The C++-only
commands do not require Python or Cython. To check that
the installed CMake package and FetchContent both work for a consumer
project, run:

```shell
./main.sh test-package
```

### Benchmarking

To run the C++ benchmarks, run:

```shell
./main.sh benchmark
```

Arguments after `benchmark` go to the Catch2 runner, such as a test name.
To benchmark Python queries, order entry/cancellation, and matching:

```shell
./main.sh benchmark-python --loops 100000 --repeats 9 --output /tmp/lob-before.json
# Rebuild after changing the bindings, then compare on the same machine.
./main.sh benchmark-python --loops 100000 --repeats 9 --compare /tmp/lob-before.json
```

The runner reports median and minimum/maximum nanoseconds per Python method
call, checks final book state outside the timed region, and compares matched
runs. See [Python bindings](docs/development/python-bindings.md) for the build
contract and measurement procedure.

Each measured iteration uses an independent empty book, with book construction
and cleanup outside the timed workload. Results from the earlier benchmarks
that reused book state are not directly comparable.

### Build Outputs

Native and Python build outputs go to the root `.build/`: the CMake build in
`.build/cmake/`, Python builds in `.build/python/`, and distributions from
`./main.sh deployment` in `.build/dist/`. Build the technical report with
`make -C docs/whitepaper`; its PDF is
`docs/whitepaper/.build/limit-order-book.pdf`. Both `.build` directories are
ignored by Git. To remove the build outputs, run:

```shell
./main.sh clean
```

### Releasing

The [CI workflow](.github/workflows/ci.yml) runs the C++ suites and the
installed-package and FetchContent checks on Linux, macOS, and Windows, and
the Python suite and distribution builds for CPython 3.13 and 3.14 on the same
platforms. It also builds the technical report from the archived evidence.
It runs on pull requests, pushes to `master`, and tag pushes.

To release, update `VERSION` in the `project()` call of
[CMakeLists.txt](CMakeLists.txt), which sets both the C++ package and the
Python package versions, then push a tag that matches it, optionally prefixed
with `v`. Tag CI waits for the report and package checks, then creates or
updates a GitHub release with the distributions and `limit-order-book.pdf`.
Manually published releases also build and attach their tag's report through
the [report workflow](.github/workflows/report.yml). Publish that release to
start the
[Publish to PyPI](.github/workflows/publish.yml) workflow, which builds the
sdist and wheels and uploads them through the `pypi` environment's trusted
publisher. A release created by the CI workflow's token does not trigger
another workflow, so if publishing does not start, dispatch the workflow
manually against the release tag:

```shell
gh workflow run publish.yml --ref <tag>
```

## Citation

Please use the same `limit-order-book` citation for the software and its
[technical report](docs/whitepaper/README.md), including the implementation
analysis and measurements. They are one project with one preferred citation.

Download [CITATION.bib](CITATION.bib) or copy the BibTeX entry below.
[CITATION.cff](CITATION.cff) provides the same preferred citation for GitHub's
**Cite this repository** feature. Please also identify the software version or
commit used in your research, and the report revision and evidence manifest
when referring to measurements.

```bibtex
@misc{kauten-limit-order-book-2019,
  author = {Christian Kauten},
  howpublished = {GitHub},
  title = {{L}imit {O}rder {B}ook: A Price-Time Priority Matching Engine for {C++}, {C}, and {P}ython},
  URL = {https://github.com/Kautenja/limit-order-book},
  year = {2019},
}
```

## Licensing

The repository declares the [MIT License](LICENSE). See
[LICENSING.md](LICENSING.md) for the scope of that grant, the licenses of the
vendored dependencies, and the third-party reference material in
`docs/references/`.
