# Doubly Linked List

[![build-status][]][build-server]

[build-status]: https://travis-ci.com/Kautenja/doubly-linked-list.svg
[build-server]: https://travis-ci.com/Kautenja/doubly-linked-list

# Roadmap

- [x] push back
- [x] append
- [x] remove
- [x] is_empty
- [x] count
- [ ] find

# Usage

Simply add [include/doubly_linked_list.hpp](include/doubly_linked_list.hpp) to
your C++ project either by copying directly or using git submodules.

## Testing

To compile and run the [test suite](test):

```shell
scons test
```

## Benchmarking

To run benchmarks:

```shell
scons benchmark/benchmark_dll.cpp
```
