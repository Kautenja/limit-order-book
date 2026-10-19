# Reproducible robin-map Vendoring

Created: 2026-10-19
Status: COMPLETE

## Goal

Keep robin-map self-contained while making its provenance and updates
reproducible. Evaluate v1.3.0 against the existing 0.6.1-labelled snapshot with
correctness tests and repeated benchmarks before choosing the retained version.

## Scope

-   Retain only upstream headers and the license in `vendor/robin-map/`.
-   Record the release, exact commit, and content checksums in a manifest.
-   Provide an update tool and durable maintenance instructions.
-   Validate C++14, C/Python behavior, installed and FetchContent consumers,
    and source/wheel distributions.
-   Compare both versions with identical sources, compiler flags, and workloads.

## Non-Goals

-   Change book algorithms, public interfaces, CMake targets, or option defaults.
-   Replace the hash map, introduce runtime downloads, or require a submodule.
-   Rewrite archived report evidence, publish packages, or commit user work.

## Acceptance Criteria

-   Vendored headers and license match a pinned upstream commit byte for byte.
-   Repeating the import produces no changes; offline verification detects
    missing, modified, and unexpected files.
-   Failed source acquisition or checksum validation leaves the snapshot intact.
-   Existing installation paths and packaged license notices remain available.
-   Tests and repeated timing results support an explicit upgrade decision,
    with host limitations and noisy measurements recorded here.

## Validation Commands

```shell
python3 scripts/update_robin_map.py --check
python3 scripts/update_robin_map.py --source /tmp/robin-map-upstream
./main.sh test
python3 -m unittest discover -s scripts -p 'test_*.py'
./main.sh test-package
./main.sh benchmark '~[bst-benchmark]' --benchmark-samples 20 --benchmark-resamples 1000
./main.sh benchmark-python --loops 100000 --repeats 9 --output /tmp/lob-robin-python.json
./main.sh deployment
git diff --check
```

Keep benchmark captures and source downloads outside the checkout. Record
exact comparison commands, results, distribution inspection, documentation
checks, and the retained version below. Preserve pre-existing changes.

## Provenance And Implementation

Evaluated on 2026-10-19. The old copy's CMake file reported 0.6.1, but three
headers differ from the v0.6.1 release tag. All four headers and the license
match upstream commit `7d079e8cb33ab93f1d42c5f3f84c8688932b6d55` byte for byte.
The baseline in this evaluation is that actual committed snapshot.

The candidate is upstream v1.3.0 at
`188c45569cc2a5dd768077c193830b51d33a5020`. Its four headers and license are
imported unchanged; `vendor/robin-map.json` pins their SHA-256 digests. The
upstream build files, tests, documentation, CI files, and debugger visualizer
are removed. The book's CMake include and install rules need no changes.

`scripts/update_robin_map.py` reads pinned Git blobs from a caller-supplied
clone. A normal import checks all checksums before writing; an explicit
version/commit update generates new hashes. Offline checking needs only
Python's standard library. The maintenance guide records the workflow and
C++14 boundary; CI checks both snapshot integrity and importer behavior.

No first-party matching implementation or public method changed. The existing
namespace integration was committed externally during this task as `7550e3b`;
this task did not commit it or alter those changes. The evaluated core source
hashes are identical between baseline and candidate except for the four
vendored headers.

## Correctness Evidence

-   Baseline `./main.sh test`: 98 C++ cases and 28 Python tests passed.
-   Candidate `./main.sh test`: 99 C++ cases and 28 Python tests passed.
    The additional scenario exercises 256 colliding price levels on both
    sides, hash growth, deletion/reinsertion, FIFO fills, aggregates, and
    stability of borrowed order/limit pointers. Compiling that scenario
    separately against the saved baseline also passed its 5,652 assertions.
-   Both versions passed the report reference model on seeds 1 through 20,
    with 10,000 events each. Their check summaries are identical: 200,000
    seeded events per version, covering limits, markets, cancels, reductions,
    clears, and duplicate IDs.
-   `./main.sh test-package`: all five C++/C checks passed across installation
    and header-only/C-enabled FetchContent paths.
-   All six importer tests passed for exact committed-byte reproduction, preservation
    of sibling dependencies, pruning, altered/missing/extra files, checksum
    rejection before mutation, invalid version/commit rejection, and offline
    checks after the upstream clone is removed, and a Git checkout with
    `core.autocrlf=true`. The snapshot has a `-text` attribute to prevent
    platform-dependent byte conversion. Reimporting the actual pinned clone
    also produced identical content.

## Benchmark Protocol

Host: macOS 26.6.2 arm64, Apple Clang 21.0.0, CPython 3.14.2. The CPU model
could not be read in the sandbox. No CPU pinning or background-load isolation
was available. No Linux or Windows execution was performed locally.

Catch2 used Release, C++14, `-O3 -DNDEBUG -Wall -pedantic`. All five book
benchmark cases (19 measurements) passed on both versions with 20 samples
and 1,000 resamples. These are supporting measurements; the comparison below
uses repeated fresh processes in the report study. The study used the same
flags plus `-Wextra`; Python used the normal CMake Release extension.

For each version, before replacing its headers, the initial study command was:

```shell
python3 docs/whitepaper/evaluate.py --output /tmp/lob-robin-study --repeats 9 --events 10000 --seeds 20 --budget 65536
```

The actual captures used separate `baseline-study` and `candidate-study`
directories under one temporary directory. Each included source hashes,
compiler/build commands, raw observations, and reference-model results.
Workload permutation seed was 20261018, with one warmup per fresh process.

The initial comparison showed overlapping ranges except shuffled insertion
at 4,096 levels, whose candidate median was 3.8% higher. A follow-up saved
each study executable and its matching C shared library, then measured all
24 non-clock configurations in 11 alternating baseline/candidate rounds.
Configuration order used Python `random.Random(20261019).shuffle` each round;
the first round ran baseline then candidate for each configuration, and the
order reversed each round. Each invocation used:

```shell
./study WORKLOAD SIZE 20261018 262144
```

On macOS, each saved executable ran in its own temporary directory with its
matching library at `.build/report/liblob_report.dylib`, the load path verified
with `otool -L`. This prevents the baseline C measurements from loading the
candidate library. All native builds and other validation stopped during
these paired measurements, except a lightweight importer-test run that
overlapped the final round. Discard that entire round for both versions;
the table below reports the first ten complete pairs per configuration,
with five rounds in each version order.

## Benchmark Results And Decision

Retain v1.3.0. It preserves C++14 and passes the integration/reference checks,
while bringing upstream correctness fixes and a reproducible source pin.
In particular, upstream [v1.2.0 release notes][fixes] describe a fix for very
long collision chains. This evaluation validates normal integration and
colliding-price behavior, not every upstream bug or possible input stream.

The paired measurements below are nanoseconds per insertion for ascending,
descending, and shuffled; per add/cancel pair for `existing_*`; and per market
request for `*_sweep`. Ranges are process-sample minima and maxima; change is
the candidate median relative to the baseline median. Positive means slower.

| Workload     | Size | Old med. |     Old range | New med. |     New range | Change |
| ------------ | ---: | -------: | ------------: | -------: | ------------: | -----: |
| ascending    |   16 |     72.3 |     69.0-73.7 |     72.5 |     69.8-78.2 |  +0.2% |
| ascending    |  128 |    119.8 |   115.6-122.5 |    120.4 |   115.1-122.0 |  +0.5% |
| ascending    | 1024 |    593.8 |   571.8-806.8 |    595.3 |   571.9-912.2 |  +0.2% |
| ascending    | 4096 |   4316.5 | 3979.3-4873.4 |   4184.7 | 3553.4-4436.4 |  -3.1% |
| descending   |   16 |     72.5 |     69.3-75.5 |     73.4 |     69.9-77.8 |  +1.3% |
| descending   |  128 |    119.4 |   115.7-122.9 |    121.0 |   118.8-121.9 |  +1.3% |
| descending   | 1024 |    595.5 |   571.3-602.8 |    593.2 |  590.6-1249.6 |  -0.4% |
| descending   | 4096 |   4454.4 | 3377.3-4564.9 |   4044.1 | 3532.8-4547.1 |  -9.2% |
| existing_c   |   16 |     45.5 |     45.0-46.0 |     45.6 |     44.7-46.1 |  +0.2% |
| existing_c   |  256 |     45.3 |     44.6-46.4 |     45.4 |     44.5-46.5 |  +0.1% |
| existing_c   | 4096 |     45.7 |     43.6-47.2 |     45.9 |     43.6-46.8 |  +0.5% |
| existing_cpp |   16 |     44.2 |     42.4-44.9 |     44.1 |     42.4-44.8 |  -0.2% |
| existing_cpp |  256 |     44.0 |     42.2-44.9 |     44.5 |     42.2-46.4 |  +1.2% |
| existing_cpp | 4096 |     44.8 |     42.5-47.4 |     44.5 |     42.3-45.9 |  -0.6% |
| fifo_sweep   |    1 |     50.5 |     45.2-51.2 |     50.4 |     46.1-51.2 |  -0.1% |
| fifo_sweep   |   16 |    318.1 |   313.6-327.4 |    314.7 |   307.1-323.5 |  -1.1% |
| fifo_sweep   |  256 |   4411.3 | 4244.3-4574.9 |   4418.4 | 4269.0-5395.1 |  +0.2% |
| level_sweep  |    1 |     50.6 |     45.6-76.9 |     50.3 |     45.8-73.6 |  -0.4% |
| level_sweep  |   16 |    575.6 |   555.0-591.6 |    577.7 |   554.1-594.0 |  +0.4% |
| level_sweep  |  256 |   9491.6 | 9136.2-9601.7 |   9519.9 | 9147.9-9776.2 |  +0.3% |
| shuffled     |   16 |     66.5 |     65.2-67.0 |     67.1 |     65.1-68.5 |  +1.0% |
| shuffled     |  128 |     65.2 |     61.9-65.6 |     66.5 |     65.3-67.8 |  +2.0% |
| shuffled     | 1024 |     80.3 |     79.0-81.3 |     81.6 |     81.0-83.8 |  +1.5% |
| shuffled     | 4096 |    106.2 |   104.1-106.9 |    108.2 |   106.3-112.4 |  +1.9% |

All paired sample ranges overlap. Shuffled insertion medians increased
1.0-2.0%, including 1.9% at 4,096 levels; this small observed cost is retained
as a limitation of the upgrade. Existing-level add/cancel and sweep medians
changed by -1.1% to +1.2%. Sorted 4,096-level insertions have broad ranges;
their lower medians do not establish a speedup. This is a maintenance and
correctness update, with no general performance improvement claimed.

Python used 100,000 loops and nine samples per workload. Both versions passed
the runner's state assertions and metadata checks. Medians/ranges in ns per
public call were:

| Workload        | Old median | Old range   | New median | New range   |
| --------------- | ---------: | ----------- | ---------: | ----------- |
| Queries         |       22.5 | 22.0-22.9   |       22.5 | 22.5-26.3   |
| Existing limit  |       63.7 | 61.3-64.2   |       63.5 | 63.1-66.4   |
| New limit       |       74.6 | 73.2-77.1   |       77.2 | 74.8-85.4   |
| Crossing limits |       98.4 | 96.4-100.9  |       98.4 | 97.8-102.8  |
| Market sweep    |       83.7 | 81.5-85.4   |       85.4 | 83.9-88.6   |
| Mixed events    |       43.5 | 43.0-44.2   |       44.3 | 43.2-52.2   |

All Python sample ranges overlap. These sequential Python measurements are
supporting evidence and do not isolate small binding-level changes.

[fixes]: https://github.com/Tessil/robin-map/releases/tag/v1.2.0

## Distribution Validation And Completion

Completed on 2026-10-19. The retained vendor directory has five files and
120,606 content bytes, down from 20 files and 309,231 bytes. The sibling
manifest and first-party maintenance scripts are outside the snapshot.
No CMake target, option default, public method, or installation path changed.

`./main.sh deployment` ran but failed while pip tried to download Cython and
scikit-build-core into its isolated environment: network access was unavailable.
The existing development environment supplied both dependencies for an offline
fallback, selecting the available Make generator instead of requiring Ninja:

```shell
CMAKE_GENERATOR='Unix Makefiles' PIP_NO_INDEX=1 .venv/bin/python -m build --no-isolation --outdir .build/dist
.venv/bin/python -m venv /tmp/lob-robin-wheel-env
/tmp/lob-robin-wheel-env/bin/python -m pip install --no-index --no-deps .build/dist/*.whl
/tmp/lob-robin-wheel-env/bin/python -I test/package/python.py
```

The offline build produced the sdist and then built the macOS arm64 CPython
3.14 wheel from that sdist. The isolated installed wheel passed all 28 Python
tests. Archive inspection verified the exact five-file snapshot and every
checksum in the sdist, the manifest and import tool, exclusion of importer
tests, and the exact upstream license in the wheel. Re-running the package
consumer checks passed all five cases; the installed headers and license
also match every pinned checksum.

An initial archive-download escalation could not run because automatic
approval review reported model capacity exhaustion, not a safety decision.
Sandboxed Git clones succeeded instead and supplied both pinned sources.
The normal isolated dependency-installation path remains unverified; no
Linux or Windows execution or remote CI run is claimed. The local Git
line-ending test is not a substitute for Windows CI.

Relative documentation links resolved, removed vendor paths have no live
references, the guide and results tables were reviewed through Pandoc's
rendered plain-text output, and `git diff --check` passed. Temporary upstream
clones, executables, benchmark captures, previews, and the wheel-test
environment were removed after recording the evidence. Distribution outputs
remain under `.build/dist/`. No commit, push, or publication was performed.

At the final status check, concurrent work had staged a version-only change
from 3.0.0 to 3.1.0 and dated the changelog section. Those staged edits were
left intact. The distribution checks above used 3.0.0 metadata before that
version change; the library implementation and compiler settings are unchanged.

On 2026-10-19, the user requested committing and pushing the robin-map work
on `2026-10-19-updates`. The version bump was already a separate commit,
`29888a3`. The pre-commit snapshot integrity check and `git diff --check`
passed; the validation results and limitations above still apply.
