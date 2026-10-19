# Python Bindings

The Python package calls the header-only C++ book through Cython. The
standalone C API remains a separate consumer interface. Both bindings must
preserve matching behavior and the widths defined in `structures.hpp`.

## Sources And Ownership

-   `limit_order_book/_book.pxd` declares the C++ types and methods. Use the
    `UID`, `Quantity`, `Price`, `Volume`, and `Count` aliases with their exact
    `uint64_t` or `uint32_t` widths, including on Windows.
-   `limit_order_book/_native.pyx` implements the Python methods and owns a
    `LOB::LimitOrderBook`. Allocate it in `__cinit__`, then clear and delete
    it in `__dealloc__`. The C++ book's price levels need explicit clearing.
    Cleanup must also work when Python subclass initialization fails.
-   `limit_order_book/limit_order_book.py` exposes `LimitOrderBook` as a Python
    subclass, preserving imports, subclassing, attributes, and weak references.

Keep the GIL during scalar book operations. The book is mutable and has no
internal locking. Mark C++ calls that can throw with `except +` so Cython
translates exceptions instead of letting them escape into the interpreter.

Preserve the existing public signatures, keywords, `price=None` defaults,
truth-value side conversion, duplicate-ID behavior, and ignored cancellation
of unknown IDs. Integer inputs use the integer-index protocol and wrap to the
unsigned C++ width, matching the former ctypes behavior. Do not accidentally
accept floats by using Cython's ordinary numeric argument conversion. Invalid
integer inputs raise `TypeError`. Returned prices and volumes are Python
integers and must retain all 64 bits; `has` returns a Python boolean.

## Build Contract

`LOB_BUILD_PYTHON_LIBRARY` retains its existing name and defaults, but now
builds `limit_order_book_native`, whose import name is
`limit_order_book._native`. CMake finds the chosen Python interpreter and its
development module, then runs that interpreter's Cython to generate C++ in
the build directory. Generated C++ and binaries must not be committed.

`pyproject.toml` installs Cython in the isolated source-build environment.
`requirements.txt` supplies it for `./main.sh install`. Wheels contain a
native extension and need neither Cython nor the standalone C library at
runtime. Keep both `.pyx` and `.pxd` sources in the sdist. Use C++14 and the
same portable Release optimization settings as other first-party targets.

`./main.sh test-python` builds and copies the extension into the checkout.
`./main.sh test` additionally builds and runs the C++ suite. Pure C++ commands
disable the Python target. FetchContent defaults remain header-only; Python
discovery must stay inside the Python build option. Check both C++ and C
consumers with `./main.sh test-package` after build changes. After installing
an isolated wheel, run that environment's Python with
`python -I test/package/python.py` to exercise the full suite against the
installed package. The existing CI matrix runs this check for every wheel.

## Performance Measurements

`benchmark/benchmark_python.py` measures Python loops over the public class.
Do not move those loops into Cython when evaluating binding overhead. Setup,
final state assertions, and object cleanup are outside the timed region.
Each sample starts with a fresh book; garbage collection is disabled only
during timing and restored afterward.

Run from the repository root on an otherwise idle machine:

```shell
./main.sh benchmark-python --loops 100000 --repeats 9 --output /tmp/lob-before.json
# Apply the change and rebuild through the same command.
./main.sh benchmark-python --loops 100000 --repeats 9 --output /tmp/lob-after.json --compare /tmp/lob-before.json
```

Use identical Python, compiler, optimization flags, warmups, loop counts,
and repeat counts. The runner checks matching Python/platform/workload
metadata; verify compiler flags separately. Record the host, flags, full
sample ranges, median speedup, and limitations in the owning spec. Repeat
in reverse order against a saved baseline when practical. Overlapping sample
ranges do not demonstrate a clear speedup. Microbenchmarks establish the
performance of these workloads on that host, not all trading applications.

Keep temporary baseline packages and captures outside the repository and
remove them once the measurements are recorded.
