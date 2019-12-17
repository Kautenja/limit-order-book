# Binary Search Tree

[![build-status][]][build-server]

[build-status]: https://travis-ci.com/Kautenja/binary-search-tree.svg
[build-server]: https://travis-ci.com/Kautenja/binary-search-tree

# Roadmap

- [x] insert node
- [x] remove node
- [x] find node with minimum key
- [x] find node with maximum key
- [ ] find node with given key

# Usage

Simply add [include/binary_search_tree.hpp](include/binary_search_tree.hpp) to
your C++ project either by copying directly or using git submodules.

## Testing

To compile and run the [test suite](test):

```shell
scons test
```

## Benchmarking

To run benchmarks:

```shell
scons benchmark/benchmark_bst.cpp
```
