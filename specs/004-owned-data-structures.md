# Own The Intrusive Data Structures

Created: 2026-10-19
Status: COMPLETE

## Goal

Make this repository the sole maintenance home for Christian Kauten's binary
search tree and doubly linked list, under the project's MIT license.

## Scope

-   Fetch both upstream repositories and import their headers into `include/`.
-   Integrate their tests with Catch2 v3 and preserve useful tree benchmarks.
-   Remove the two vendor trees and their standalone build configuration.
-   Update installation, contribution rules, and report source references.

## Non-Goals

-   Change algorithms, node layout, C/C++/Python APIs, or CMake targets/options.
-   Change `robin-map`, rewrite historical measurements, or claim a speedup.
-   Delete remote repositories, tag, or publish packages.

## Acceptance Criteria

-   The two data structures build from first-party headers with no dependency
    on their former repositories or vendor paths.
-   Existing `BST`/`DLL` names and installed header names remain available.
-   Imported tree and list tests run in the normal C++ test suite.
-   C++, Python, installed-package, FetchContent, and distribution checks pass.
-   Current documentation describes local ownership; archived evidence retains
    the original evaluated source paths and hashes.

## Validation Commands

Run from the repository root:

```shell
./main.sh benchmark --benchmark-samples 10 --benchmark-resamples 1000
./main.sh test
./main.sh test-package
./main.sh deployment
python3 docs/whitepaper/evaluate.py --output /tmp/lob-import-study --repeats 1 --events 100 --seeds 1 --budget 128
make -C docs/whitepaper
git diff --check
```

Inspect distribution contents, check relative documentation links, and review
rendered documentation. Record actual results and limitations below.

## Import Sources

Fetched directly on 2026-10-19. Both upstream snapshots match the existing
vendored first-party files byte for byte:

-   `Kautenja/binary-search-tree`: `9b00d9c6246cd6fb1c69983c9ee2bf95f509e1e0`.
-   `Kautenja/doubly-linked-list`: `9f7f752685fd6b9cae25236a9467a734df74d428`.

## Completion Evidence

Completed on 2026-10-19, on macOS arm64 with Apple Clang 21.0.0 and CPython
3.14.2. No Linux or Windows run was performed locally.

-   Imported both headers from the fetched snapshots. After removing comments
    and whitespace, their code matches upstream exactly. Existing names,
    include guards, node layouts, and matching behavior are unchanged.
-   Retained all 26 upstream tree cases and six list cases, with all 671
    ordinary `REQUIRE` assertions. Switched to Catch2 v3, gave CTest readable
    case names, fixed the key-constructor test invocation, and used stack-owned
    test nodes instead of leaking allocations.
-   Integrated the three tree benchmark workloads with Catch2 v3, independently
    owned node storage per iteration, and untimed preparation/cleanup. The old
    list benchmark contained no workloads. Removed both vendor directories,
    their SCons/Travis configuration, and separate license-install rules.
-   Updated the maintenance guides, CMake includes, copying instructions,
    licensing guide, and report references. `robin-map` and archived report
    evidence are unchanged.
-   `./main.sh test` passed all 98 C++ cases and 28 Python cases without compiler
    warnings. `./main.sh test-package` passed all five C++/C checks across
    installed-package, header-only FetchContent, and FetchContent with the C API.
    Installed headers retain their filenames; installed licenses contain only
    this project's documents and the `robin-map` notice.
-   `./main.sh benchmark --benchmark-samples 10 --benchmark-resamples 1000`
    passed before and after the move (five existing workloads before; those
    five plus three imported tree workloads after). Both builds used C++14,
    Release, `-O3 -DNDEBUG`, and the same compiler. After consolidating untimed
    fixture assertions, the final tree runner also passed:
    `./main.sh benchmark '[bst-benchmark]' --benchmark-samples 5 --benchmark-resamples 1000`.
    These are smoke runs, not repeated process-level performance evidence;
    no speedup or regression claim is made. The imported benchmarks' timing
    boundaries differ from their old standalone harness.
-   The reduced evaluation command above passed its 100-event seeded check and
    completed every timing configuration. Its new manifest includes both
    first-party header paths and no retired vendor paths. This was a driver
    smoke check, not a replacement for the archived study.
-   `make -C docs/whitepaper` produced an eight-page report with no LaTeX
    warnings or overfull/underfull boxes. All eight rendered pages were
    inspected. Relative documentation paths resolved, the new ownership guide
    and revised licensing guide were rendered offline and visually reviewed,
    and `git diff --check` passed.

### Distribution Validation And Limitations

The first benchmark configure could not fetch Catch2 in the network sandbox;
the approved retry fetched it and passed. Later, automatic approval review for
`./main.sh deployment` failed twice because the review model was at capacity.
The command was not executed, and the service made no safety determination.
The normal isolated deployment build therefore remains unverified for this
change.

An offline build used the already-installed build dependencies within the
sandbox. The default generator requested missing Ninja, so the successful
command selected the available Make generator:

```shell
CMAKE_GENERATOR='Unix Makefiles' PIP_NO_INDEX=1 .venv/bin/python -m build --no-isolation --outdir .build/dist
.venv/bin/python -m venv /tmp/lob-import-wheel-env
/tmp/lob-import-wheel-env/bin/python -m pip install --no-index --no-deps .build/dist/*.whl
/tmp/lob-import-wheel-env/bin/python -I test/package/python.py
```

This built the source distribution, then built the macOS arm64 CPython 3.14
wheel from that source archive. The installed wheel passed all 28 Python
tests. Archive inspection confirmed both first-party headers in the sdist,
the project and `robin-map` licenses in both distributions, and no retired
vendor trees or standalone tree/list licenses. Browser preview of local
Markdown HTML was blocked by URL policy; offline Pandoc rendering was used
for visual review instead.

Temporary clones, captures, previews, and the wheel-test environment were
removed. No package publication or remote repository deletion was performed.
On 2026-10-19, the user requested committing and pushing the completed
migration on the current `2026-10-19-updates` branch.
