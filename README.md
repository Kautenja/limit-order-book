# Limit Order Book

[![build-status][]][build-server]
[![PackageVersion][pypi-version]][pypi-home]
[![PythonVersion][python-version]][python-home]
[![Stable][pypi-status]][pypi-home]
[![Format][pypi-format]][pypi-home]
[![License][pypi-license]][license]

[build-status]: https://travis-ci.org/Kautenja/limit-order-book.svg
[build-server]: https://travis-ci.org/Kautenja/limit-order-book
[pypi-version]: https://badge.fury.io/py/limit-order-book.svg
[pypi-license]: https://img.shields.io/pypi/l/limit-order-book.svg
[pypi-status]: https://img.shields.io/pypi/status/limit-order-book.svg
[pypi-format]: https://img.shields.io/pypi/format/limit-order-book.svg
[pypi-home]: https://badge.fury.io/py/limit-order-book
[python-version]: https://img.shields.io/pypi/pyversions/limit-order-book.svg
[python-home]: https://python.org
[license]: ./LICENSE

This is an implementation of the limit order book with a price-time priority
matching policy for reception of market data streams. There are APIs for C++,
C, and Python.

## Usage

### C++

Simply add [include/*.hpp](include) to your C++ project either by copying
directly or using git submodules.

### C

A C-level API is defined in [include/lib_lob.hpp](include/lib_lob.hpp).

### Python

The preferred Python installation of `limit-order-book` is from `pip`:

```shell
pip install limit-order-book
```

## Usage Caveats

### Windows

You'll need to install the Visual-Studio 17.0 tools for Windows installation.
The [Visual Studio Community](https://visualstudio.microsoft.com/downloads/)
package provides these tools for free.

## Development

### Testing

To run all the unit-test suites, run:

```shell
make test
```

#### C++

To run the C++ unit-test suite, run:

```shell
scons test
```

#### Python

To run the Python unit-test suite, run:

```shell
python -m unittest discover .
```

### Benchmarking

#### C++

To run the C++ benchmark code, run:

```shell
scons benchmark
```
