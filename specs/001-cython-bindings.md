# Cython Python Bindings

Created: 2026-10-18
Status: COMPLETE

Replace the Python package's ctypes calls with a Cython extension that calls
the existing C++ book directly, following nes-py's native binding approach.
Measure the Python interface before and after with identical workloads.

## Scope

-   Preserve `LimitOrderBook` imports, methods, keywords, defaults, side
    semantics, duplicate IDs, unknown cancellation, and integer widths.
-   Remove the low-level ctypes loader; the user confirmed that only the
    `LimitOrderBook` class needs compatibility.
-   Integrate Cython with CMake, scikit-build-core, development commands,
    source distributions, wheels, and existing CI.
-   Add repeatable Python benchmarks and binding regression coverage.
-   Update contributor documentation and record validation and measurements.

## Non-Goals

-   Change matching algorithms, C++ headers, the standalone C API, or vendors.
-   Rename existing CMake options or change their defaults.
-   Publish or tag a release.

## Acceptance Criteria

-   Existing Python tests and added boundary/lifetime tests pass.
-   No runtime ctypes dependency or separate C library is needed by Python.
-   C++ and C consumer checks still pass without Python build dependencies.
-   The sdist builds a wheel, and an isolated installed wheel passes Python
    tests without importing from the checkout.
-   Repeated comparable Python measurements show an improvement larger than
    observed run-to-run variation for every measured workload, or any
    inconclusive result is explicitly reported.

## Validation Commands

```shell
./main.sh test
./main.sh test-package
./main.sh deployment
./main.sh benchmark-python --loops 100000 --repeats 9 --output /tmp/lob-after.json
git diff --check
```

The benchmark script is run against the unchanged ctypes package before the
migration and against the Cython package afterward using the same interpreter,
compiler, Release flags, workload sizes, warmups, and repeats. Temporary
captures and the baseline package copy live outside the repository and are
removed after results are recorded here.

## Implementation Decisions

-   The extension calls the existing C++ header directly, as nes-py's
    [`_native.pyx`](https://github.com/Kautenja/nes-py/blob/master/nes_py/_native.pyx)
    and CMake build do. Matching code and the C ABI were not changed.
-   Keep `LimitOrderBook` as a Python subclass of the extension type so
    subclasses, custom attributes, and weak references keep working.
-   Preserve unsigned wrapping and integer-index conversion. Invalid integer
    arguments now raise ordinary `TypeError` instead of the removed ctypes
    loader's `ArgumentError`; this is recorded in the changelog.
-   Clear native price levels before deleting the C++ owner, including after
    failed subclass initialization. Keep the GIL for scalar operations.
-   Preserve `LOB_BUILD_PYTHON_LIBRARY` defaults. C++-only development commands
    explicitly disable it; FetchContent does not discover Python or Cython.
-   Add installed-wheel execution of the full Python suite to the existing CI
    matrix, in addition to source-checkout tests and distribution builds.

## Benchmark Evidence

Measured on 2026-10-18 using an Apple M1 Pro (arm64), macOS 26.6.2,
CPython 3.14.2, Apple Clang 21.0.0 (`clang-2100.3.34.2`), CMake 4.2.2,
and Cython 3.3.0. The baseline is the unchanged ctypes binding at
`b177394`, copied with its built shared library before migration.

Both native builds used Release, C++14, and the same compiler options:

```text
-O3 -DNDEBUG -std=c++14 -arch arm64 -fPIC
-fvisibility=hidden -fvisibility-inlines-hidden -Wall -pedantic
```

The Cython build additionally includes Python headers; export macros differ
because it builds a Python extension. Neither uses LTO or host-tuned flags.
Each workload has 100,000 iterations, two full warmups, and nine measured
samples. All event loops run in Python. Setup and final-state checks are
excluded. The units below are nanoseconds per public method call, averaged
across the calls in each workload.

The recorded commands were:

```shell
.venv/bin/python benchmark/benchmark_python.py --package-root /tmp/lob-cython.5szs56 --label ctypes --output /tmp/lob-cython.5szs56/before.json
cmake -S . -B .build/cmake -DCMAKE_OSX_ARCHITECTURES=arm64
cmake --build .build/cmake --target limit_order_book_native
.venv/bin/python benchmark/benchmark_python.py --label cython --output /tmp/lob-cython.5szs56/after-final.json --compare /tmp/lob-cython.5szs56/before.json
.venv/bin/python benchmark/benchmark_python.py --package-root /tmp/lob-cython.5szs56 --label ctypes-recheck --output /tmp/lob-cython.5szs56/before-recheck.json
```

| Workload        | ctypes ns | Cython ns | Speedup | ctypes Recheck ns |
| --------------- | --------: | --------: | ------: | ----------------: |
| queries         |     309.0 |      21.8 |  14.17x |             307.9 |
| existing_limit  |     505.0 |      61.5 |   8.21x |             506.8 |
| new_limit       |     503.5 |      73.7 |   6.84x |             506.6 |
| crossing_limits |     643.3 |      96.9 |   6.64x |             641.8 |
| market_sweep    |     591.4 |      81.5 |   7.25x |             598.2 |
| mixed_events    |     494.2 |      46.3 |  10.68x |             492.9 |

Observed minimum-to-maximum ranges across measured samples:

| Workload        | ctypes ns   | Cython ns | ctypes Recheck ns |
| --------------- | ----------: | --------: | ----------------: |
| queries         | 307.2-498.5 | 21.6-21.9 |       306.6-310.5 |
| existing_limit  | 499.1-507.4 | 61.3-62.1 |       502.3-508.2 |
| new_limit       | 502.2-517.5 | 73.4-73.8 |       504.8-524.2 |
| crossing_limits | 636.5-651.6 | 96.7-97.2 |       639.5-644.2 |
| market_sweep    | 590.8-596.7 | 81.5-81.6 |       596.0-598.9 |
| mixed_events    | 489.3-570.1 | 46.1-47.2 |       491.9-494.6 |

Every Cython sample was faster than every ctypes sample for its workload,
including the baseline rerun after the Cython measurements. The initial
Cython run also showed separated ranges (6.71x-14.19x median speedups).
The final matched-flag run shows 6.64x-14.17x median speedups. These results
establish improvements for these six Python workloads on this host; they do
not predict other machines or complete application throughput.

## Validation Evidence

Validation on 2026-10-18:

-   Before migration, `./main.sh test-python`: all 20 existing tests passed.
    The initial sandboxed Catch2 download failed because network access was
    unavailable; the authorized network-enabled retry passed.
-   `./main.sh test`: all 65 Catch2 cases and all 28 Python tests passed.
-   `./main.sh test-package`: installed C++/C consumers passed (2 tests),
    header-only FetchContent passed (1 test), and C++/C FetchContent passed
    (2 tests). The consumer asserts that the Python extension is absent.
-   A deterministic 5,000-event replay (seed `20261018`) used generic and
    side-specific limits/markets, cancellation, duplicate IDs, and clear.
    After every event, all best prices, counts, volumes at 11 prices, and
    membership of 80 IDs were compared through a SHA-256 digest. The ctypes
    and Cython digests matched.
-   `./main.sh deployment`: the sdist and CPython 3.14 wheel built successfully
    in isolated environments; the wheel was built from the sdist.
-   Archive inspection confirmed both Cython sources in the sdist, no
    generated binaries in the sdist, one native extension in the wheel, and
    no ctypes loader or C API shared library in the wheel.
-   A fresh `.build/wheel-test` virtual environment installed only the 3.14
    wheel, then `.build/wheel-test/bin/python -I test/package/python.py`
    passed all 28 tests. Cython was absent from that environment.

-   CPython 3.13.11 was also validated with:

    ```shell
    .build/wheel-test-313/bin/python -m build --wheel --outdir .build/dist
    .build/wheel-test-313/bin/python -m pip install --no-deps .build/dist/*cp313*.whl
    .build/wheel-test-313/bin/python -I test/package/python.py
    ```

    The isolated wheel build and all 28 installed-wheel tests passed. Cython
    was absent from the runtime environment.
-   `./main.sh benchmark-python --loops 1000 --repeats 2 --warmups 1`
    passed all six workload state checks through the new command. The final
    checkout Python suite also passed all 28 tests after rebuilding.
-   `bash -n main.sh`, `git diff --check`, relative Markdown link checks, and
    Pandoc Markdown/table parsing passed. Pandoc rendered the report and
    binding guide to HTML. Visual browser inspection was skipped because
    the browser policy blocks local `file:` URLs.
-   Temporary baseline copies, replay scripts, captures, and logs were
    removed after recording the results.

Linux, Windows, and other architectures were not executed locally. The
existing CI matrix, including the new installed-wheel check, must validate
those platforms. No tag or publication was performed.
