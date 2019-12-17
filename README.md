# Limit Order Book (LOB)

[![build-status][]][build-server]
[![PackageVersion][pypi-version]][pypi-home]
[![PythonVersion][python-version]][python-home]
[![Stable][pypi-status]][pypi-home]
[![Format][pypi-format]][pypi-home]
[![License][pypi-license]](LICENSE)

[build-status]: https://travis-ci.com/Kautenja/lob.svg
[build-server]: https://travis-ci.com/Kautenja/lob
[pypi-version]: https://badge.fury.io/py/lob.svg
[pypi-license]: https://img.shields.io/pypi/l/lob.svg
[pypi-status]: https://img.shields.io/pypi/status/lob.svg
[pypi-format]: https://img.shields.io/pypi/format/lob.svg
[pypi-home]: https://badge.fury.io/py/lob
[python-version]: https://img.shields.io/pypi/pyversions/lob.svg
[python-home]: https://python.org

This is an implementation of the limit order book structure and matching
algorithm for C++ (and Python through ctypes) for market data streaming.

![Limit order book](img/limit-order-book.svg)

## Usage

### C++

Simply add [include/*.hpp](include) to your C++ project either by copying
directly or using git submodules.

### Python

The preferred Python installation of `lob` is from `pip`:

```shell
pip install lob
```

### Windows

You'll need to install the Visual-Studio 17.0 tools for Windows installation.
The [Visual Studio Community](https://visualstudio.microsoft.com/downloads/)
package provides these tools for free.

## Testing

### C++

To run the C++ unit-test suite, run:

```shell
scons test
```

### Python

To run the Python unit-test suite, run:

```shell
python -m unittest discover .
```

## Benchmarking

### C++

To run the C++ benchmark code, run:

```shell
scons benchmark
```
