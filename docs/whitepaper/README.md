# Limit Order Book Technical Report

**Limit Order Book: A Price-Time Priority Matching Engine for C++, C, and
Python**, by Christian Kauten. Technical report revision: October 19, 2026.
The report and software share the project's [2019 citation](../../CITATION.bib).
The revision date identifies this document, not a separate cited work. This
is a repository manuscript, without an assigned DOI, peer-review claim, or
publication acceptance.

The report explains how the book was built and why its representation is
useful: occupied-price trees, intrusive FIFO queues, separate order and price
indices, and cached best prices and aggregates. It places those choices beside
the 2011 How to HFT design and the QuantCup and Island sources already present
in the project's first commit in December 2019. The comparison connects each
benefit to its cost, including allocation, pointer lifetime, and tree height.

The construction account covers the now-owned `LOB::BST` and `LOB::DLL`
helpers, the vendored price index, and the shared C++ core behind C and Python.
Detailed evaluation methods and the complete timing table are in appendices.
The report distinguishes the original design, later maintenance, and the
archived measured snapshot; it claims neither a new matching algorithm nor
superiority over the historical engines.

## Read And Build

[main.tex](main.tex) is the canonical LaTeX source. In the Codex LaTeX editor,
use the repository root as the project and `docs/whitepaper/main.tex` as the
main document so the shared bibliography is included. For a command-line
build, install `latexmk`, `pdflatex`, `bibtex`, and the packages in the
preamble, then run from the repository root:

```shell
make -C docs/whitepaper
```

The PDF is written to
[.build/limit-order-book.pdf](.build/limit-order-book.pdf), beside this source
directory. From the repository root, its full relative path is
`docs/whitepaper/.build/limit-order-book.pdf`.
The build disables shell escape and typesets the checked-in evidence; it does
not run benchmarks or fetch research. LaTeX outputs stay under this directory's
ignored `.build/`. `make -C docs/whitepaper clean` removes the LaTeX outputs;
`./main.sh clean` removes both the root and report build directories.

GitHub Actions uses the same build command and attaches `limit-order-book.pdf`
to tag releases and manually published releases. Branch builds also expose it
as the `limit-order-book-report` Actions artifact. See the
[report build guide](../development/technical-report.md) for workflow details.

The source uses the two-column layout, 10-point type, 0.75-inch margins,
running headers, and restrained blue and orange palette of the RackNES and
Fourier reports. BibTeX reads the project's entry directly from
[../../CITATION.bib](../../CITATION.bib) and prior-work entries from
[references.bib](references.bib). The architecture diagram is in the
manuscript. Numerical macros and plot data are generated from the raw CSV.

## Reproduce The Study

The independent study needs Python 3, a C++14 Clang or GCC compiler, and the
first-party and vendored headers already in this repository. The driver
supports macOS and Linux; only macOS was evaluated. It does not need Cython,
Catch2, a market-data subscription, network access, or third-party Python
packages.

From the repository root, choose a **new** capture directory outside the
checkout and run:

```shell
python3 docs/whitepaper/evaluate.py --output /tmp/lob-report-study
```

The driver builds its executable and shared library in `.build/report/`,
checks 20 seeded streams of 10,000 event selections against an independent
vector model, and records nine process-level observations for each of 25 timing
configurations. It performs a workload warmup in each process and randomizes
configuration order per round. Set `CXX` to select another compiler. The
capture includes exact flags, compiler/host metadata, source hashes, check
results, and every observed timing. A source change during evaluation is an
error. Use an otherwise idle machine when reproducing timing results.

The checker compares order fields, live IDs, aggregates, best prices, and
FIFO links and endpoints after each selection. Selections include no-ops;
the archive records the selected event types, duplicate limits, and empty
cancellations/reductions. It does not certify every internal invariant or
complete equivalence between the C++, C, and Python interfaces.

The reference model and timed harness are in
[benchmark/report/study.cpp](../../benchmark/report/study.cpp). The harness
is kept separate from the existing Catch2 runner so its `main` function and
protocol do not alter normal benchmarks. It explicitly clears residual price
levels outside timing; the report explains the core's ownership limitation.
No matching source or vendored dependency was changed for the study.

The paper reports these units:

| Workload                         | Size Means       | Timing Unit            |
| -------------------------------- | ---------------- | ---------------------- |
| Ascending, descending, shuffled   | Price levels     | ns per insertion       |
| Existing level, C++ or C          | Anchored levels  | ns per add-cancel pair |
| FIFO sweep, level sweep          | Consumed orders  | ns per market request  |
| Clock control                    | One interval     | ns per empty interval  |

All reported ranges are the minimum and maximum of nine **batch averages**.
They are not confidence intervals or individual-request tail latencies. The
shuffled permutation is fixed across repeats, and no CPU pinning or load
isolation is performed. Fresh books can still reuse warm caches and allocator
state, and existing-level pairs repeatedly touch one price and ID. Python
timing and competitor comparisons are outside
this study. See the manuscript for setup, allocation, and cleanup boundaries.

## Inspect And Regenerate Evidence

[evidence/](evidence) contains the selected report observations:

-   `metadata.json`: Source hashes, compiler commands, host, seeds, and protocol.
-   `checks.json`: One correctness result and event-selection counts per stream.
-   `timings.csv`: Every process-level observation and its reporting denominator.
-   `summary.json`: Medians and minimum-maximum ranges.
-   `results.tex`, `table-rows.tex`, and price-order CSVs: Generated paper inputs.
-   `validation.json`: The ordinary repository-suite results for this report.
-   `architecture-sources.json`: Hashes of additional interface sources inspected
    for the architecture account, outside the timed C++/C study.
-   `reference-sources.json`: Hashes of the two historical engine files cited
    and analyzed in the report, without executing or modifying them.

To regenerate the checked-in numerical summaries without benchmarking:

```shell
python3 docs/whitepaper/evaluate.py --summarize docs/whitepaper/evidence
make -C docs/whitepaper
```

Keep raw observations and their metadata together. A reproduction on another
host is a new experiment; retain its identity and update the paper's host,
date, and limitations when adopting it. Do not overwrite the archived run
with timings from a different machine while retaining the original claims.
The evidence directory's `.gitattributes` preserves CSV bytes without Git
line-ending conversion.

The evaluated core comes from base revision
`3a4d254e9bda40d4347b71dbac2cd72c97aeb3b5`, which includes the Cython binding.
The study and report are additions beyond that base revision. Exact evaluated
inputs are identified by the metadata's SHA-256 hashes. The Python architecture
account describes the accompanying native binding; no Python performance
result is included in this report.

The October 19 helper integration and robin-map v1.3.0 update postdate that
study. The archive retains the old vendor paths and hash-map bytes. Its
numbers are not measurements of the updated dependency, and the ordinary
evaluation command above measures the current checkout instead.

For an exact source replay, use the original report commit
`336b2168811290b8d67a2c367ff19f946a183cdc`. All 22 inputs in the study,
architecture, and historical-reference manifests match that commit, including
the original evaluation driver. Do not overlay the current driver: its include
paths now expect the first-party helpers. With both `/tmp` destinations unused:

```shell
git worktree add --detach /tmp/lob-report-replay 336b2168811290b8d67a2c367ff19f946a183cdc
cd /tmp/lob-report-replay
python3 docs/whitepaper/evaluate.py --output /tmp/lob-report-replay-results
```

Compare the new `source_hashes` in `metadata.json` with the archived manifest
before comparing results. The new run records the report commit as its Git
revision; the matching input hashes establish equivalence to the archived
study. Different host state can change timings even when all sources match.

## Attribution And Citation

[sources.md](sources.md) maps claims to prior work and implementation sources,
including the availability limits of the archived How to HFT article. It also
identifies the QuantCup source credited to voyager and the Island2 source
credited to Joshua Levine, with source-specific comparisons and provenance
limits. Neither engine is an evaluated baseline.
Use the top-level [CITATION.bib](../../CITATION.bib) for both the software and
this report. [CITATION.cff](../../CITATION.cff) and the
[root README](../../README.md#citation) provide the same preferred citation.
When citing a measured implementation, also identify the source revision,
report revision, and evidence manifest used.

The [MIT License](../../LICENSE) covers first-party sources and this report;
vendored components and imported references retain their own attribution and
terms as described in [LICENSING.md](../../LICENSING.md).
