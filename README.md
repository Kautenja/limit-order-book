# Limit Order Book

[![build-status][]][build-server]
[![PackageVersion][pypi-version]][pypi-home]
[![PythonVersion][python-version]][python-home]
[![Stable][pypi-status]][pypi-home]
[![Format][pypi-format]][pypi-home]
[![License][pypi-license]][license]

[build-status]: https://github.com/Kautenja/limit-order-book/actions/workflows/ci.yml/badge.svg
[build-server]: https://github.com/Kautenja/limit-order-book/actions/workflows/ci.yml
[pypi-version]: https://badge.fury.io/py/limit-order-book.svg
[pypi-license]: https://img.shields.io/pypi/l/limit-order-book.svg
[pypi-status]: https://img.shields.io/pypi/status/limit-order-book.svg
[pypi-format]: https://img.shields.io/pypi/format/limit-order-book.svg
[pypi-home]: https://badge.fury.io/py/limit-order-book
[python-version]: https://img.shields.io/pypi/pyversions/limit-order-book.svg
[python-home]: https://python.org
[license]: ./LICENSE

This is a header-only C++ implementation of the limit order book with a
price-time priority matching policy for reception of market data streams. The
C++ library is the primary product; a C API and a Python package built on it
are also provided.

## Usage

### C++

The library is header-only and requires C++14. It depends on three header-only
libraries that are vendored in [vendor/](vendor) and installed with it:
`binary-search-tree`, `doubly-linked-list`, and `robin-map`. Every method
below provides the `limit_order_book::limit_order_book` target, or the include
path, so that this works:

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

#### System Installation

Install the headers, a CMake package, and a pkg-config file. This requires
CMake 3.21 or later and a C++14 compiler:

```shell
git clone https://github.com/Kautenja/limit-order-book.git
cd limit-order-book
cmake -S . -B .build/cmake -DLOB_BUILD_TESTS=OFF
cmake --install .build/cmake --prefix /usr/local
```

Use any prefix, such as `"${HOME}/.local"`, and `sudo` when the prefix needs
it. The install puts every header in `include/limit_order_book/`, the CMake
package in `share/cmake/limit_order_book/`, the pkg-config file in
`share/pkgconfig/`, and the licenses in `share/doc/limit_order_book/`. Then
use the package from CMake:

```cmake
find_package(limit_order_book 2 CONFIG REQUIRED)
target_link_libraries(my_app PRIVATE limit_order_book::limit_order_book)
```

or from any build system through pkg-config:

```shell
c++ -std=c++14 $(pkg-config --cflags limit_order_book) my_app.cpp
```

#### Copying The Headers

Copy [include/](include) and the `include/` directories of the three vendored
dependencies into your project, and add all four to the include path.

### C

The C API in [include/limit_order_book_c.h](include/limit_order_book_c.h) is
a C99 header that wraps the book in an opaque `LOB_LimitOrderBook` handle.
The same shared library backs the Python package. Build and install it with
`LOB_BUILD_C_LIBRARY=ON`:

```shell
cmake -S . -B .build/cmake -DLOB_BUILD_TESTS=OFF -DLOB_BUILD_C_LIBRARY=ON
cmake --build .build/cmake
cmake --install .build/cmake --prefix /usr/local
```

The install adds `limit_order_book_c.h` to `include/limit_order_book/`, the
`liblimit_order_book` shared library to `lib/`, and a
`limit_order_book_c.pc` pkg-config file to `lib/pkgconfig/`. Because the
package now holds a compiled library, the CMake package moves to
`lib/cmake/limit_order_book/`. Link the `limit_order_book::c` target:

```cmake
find_package(limit_order_book 2 CONFIG REQUIRED COMPONENTS c)
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
distribution and need a C++14 compiler.

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
The C++ suite runs through `ctest`; the Python suite runs against the C API
library that the CMake build copies into `limit_order_book/`. To check that
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

### Build Outputs

All build outputs go to the ignored `.build/` directory: the CMake build in
`.build/cmake/`, Python builds in `.build/python/`, and distributions from
`./main.sh deployment` in `.build/dist/`. To remove them, run:

```shell
./main.sh clean
```

### Releasing

The [CI workflow](.github/workflows/ci.yml) runs the C++ suites and the
installed-package and FetchContent checks on Linux, macOS, and Windows, and
the Python suite and distribution builds for CPython 3.13 and 3.14 on the same
platforms. It runs on pull requests, pushes to `master`, and
tag pushes.

To release, update `VERSION` in the `project()` call of
[CMakeLists.txt](CMakeLists.txt), which sets both the C++ package and the
Python package versions, then push a tag that matches it, optionally prefixed
with `v`. Tag CI creates or updates a GitHub release with the built
distributions. Publish that release to start the
[Publish to PyPI](.github/workflows/publish.yml) workflow, which builds the
sdist and wheels and uploads them through the `pypi` environment's trusted
publisher. A release created by the CI workflow's token does not trigger
another workflow, so if publishing does not start, dispatch the workflow
manually against the release tag:

```shell
gh workflow run publish.yml --ref <tag>
```

## Citation

Please cite `limit-order-book` if you use it in your research.

Download [CITATION.bib](CITATION.bib) or copy the BibTeX entry below.
[CITATION.cff](CITATION.cff) provides the same preferred citation for GitHub's
**Cite this repository** feature. Please also identify the software version or
commit used in your research.

```bibtex
@misc{limit-order-book,
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
