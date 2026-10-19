# Technical Report

Created: 2026-10-18
Status: COMPLETE

## Goal

Write a LaTeX technical report in the visual style of RackNES and Fourier,
with explicit attribution, source-grounded complexity analysis, and a
reproducible evaluation of the existing matching implementation.

## Scope

-   Add the manuscript, inline bibliography, architecture figure, evidence,
    reproduction instructions, and report citation under `docs/whitepaper/`.
-   Add an isolated study harness under `benchmark/report/`, without changing
    the matching implementation or the existing Catch2 benchmark runner.
-   Compare the C++ implementation with an independent reference model,
    measure controlled workloads with repeated process runs, and archive
    source hashes, environment metadata, and individual timing observations.
-   Build the PDF in `.build/report/` and inspect every rendered page.
-   Link the report from the documentation index and public README.

## Non-Goals

-   Introduce a new matching algorithm or claim priority for established
    price-tree, FIFO-queue, or hash-index designs.
-   Modify the concurrent Cython migration, vendored libraries, or public API.
-   Claim production exchange readiness, financial-market realism, or measured
    Linux/Windows behavior from this macOS evaluation.
-   Push, publish, tag, or submit the manuscript.

## Acceptance Criteria

-   The report distinguishes inherited ideas, project implementation, source
    analysis, and measured results, and identifies a reproducible snapshot.
-   A deterministic reference model checks state after every generated event.
-   Timed repetitions perform the stated operation on fresh or restored state;
    setup, validation, and cleanup boundaries are documented.
-   All reported numbers derive from archived raw results and a regeneration
    command; dispersion and measurement limits accompany timing summaries.
-   LaTeX compiles without unresolved references or layout overflow, and the
    final PDF passes visual inspection.

## Validation Commands

Run from the repository root:

```shell
python3 docs/whitepaper/evaluate.py --output /tmp/lob-report-study
python3 docs/whitepaper/evaluate.py --summarize docs/whitepaper/evidence
./main.sh test
make -C docs/whitepaper
git diff --check
```

The study records the exact compiler command and hashes of its inputs. Raw
captures remain outside the repository until selected as durable report
evidence; only build products belong in `.build/`.

## Completion Evidence

Completed and critically reviewed October 18, 2026. The eight-page manuscript
uses the two-column RackNES/Fourier presentation, with a full-width timing
appendix. It credits How to HFT, the owner-authored tree/list libraries,
Tessil, Cython, voyager's QuantCup submission, and Joshua Levine's Island2
source. It distinguishes source analysis from measured results.

The selected evaluation uses base revision
`3a4d254e9bda40d4347b71dbac2cd72c97aeb3b5`, including the native Cython
binding and the independently corrected Catch2 benchmarks. A final source
hash check detected concurrent checkout updates, so the report and evaluation
were refreshed against this revision. The report, study, and README links
are additions beyond that evaluated base revision. This task changed no
matching implementation, existing Catch2 benchmark, vendored library, or
imported historical engine.

### Critical Review Findings And Fixes

-   The abstract overstated causal interpretation of the insertion timing.
    It now reports a ratio of medians and agreement with source analysis;
    traversal, allocation, and cache effects were not isolated experimentally.
-   State checking did not directly compare FIFO order before a fill exposed
    a difference. The checker now verifies order identities, predecessor and
    successor links, containing levels, and queue endpoints against the
    reference model. Claims identify the properties checked and exclude
    exhaustive internal-invariant and cross-language verification.
-   Generated selections were easy to confuse with actual mutations. The
    archive now records event-type counts, 12,153 empty cancellations or
    reductions, and 905 duplicate limit submissions across 200,000 selections.
-   Fresh logical books do not imply cold hardware or allocator state, and
    existing-level pairs reuse one hot price and ID. The report now states
    these limits, preserves every timing observation, and distinguishes batch
    averages from request-tail latency and confidence intervals.
-   The historical engines lacked explicit citations and technical treatment.
    The report now contrasts QuantCup's price array, fixed arena, and deferred
    cancellation with the library's tree and immediate unlinking, and describes
    Island's indexed records, reuse list, and surrounding messages. It credits
    source authors without claiming code ancestry, a contest result, or
    comparative speed. Historical source bytes have a separate hash manifest.
-   The Island copyright year was not sufficient provenance for a version date.
    The report distinguishes the local 2.40/1996 identifiers from Levine's
    release-page description of a December 15, 2003 snapshot. Remote source
    byte identity remains unverified because the linked file failed decoding.
-   The current Catch2 suite already prepares fresh state, so the manuscript
    no longer describes its historical identifier-reuse issue as current.
    The README also gives a separate-checkout recipe for replaying the exact
    evaluated base with the report additions.

### Validation

-   `python3 docs/whitepaper/evaluate.py --output /tmp/lob-report-reviewed-snapshot`:
    Passed 20 seeded streams of 10,000 selections, including the strengthened
    FIFO checks. Collected 225 observations: 25 configurations, nine processes
    each. The selected raw evidence and its source hashes are archived.
-   Independently checked all source manifests, per-seed selection totals,
    timing row counts, rounds, denominators, checksums, medians, and ranges.
    `python3 docs/whitepaper/evaluate.py --summarize docs/whitepaper/evidence`
    recreated every generated numerical input byte-for-byte.
-   A temporary C++14 review probe verified the manuscript's hand-calculated
    partial-fill example against both implementations. It also deliberately
    reordered a queue without changing live order fields or totals; the new
    comparator rejected it with a FIFO-predecessor diagnostic. The probe
    compiled with `-O3 -DNDEBUG -Wall -Wextra -pedantic` without warnings.
-   `./main.sh test`: Passed all 66 C++ tests and 28 Python tests on the selected
    revision. No Linux or Windows validation is claimed.
-   `make -C docs/whitepaper` and the desktop LaTeX compiler succeeded. The PDF
    has eight pages, resolved references, and no overfull boxes. All final
    pages were rendered and visually reviewed, including the bibliography,
    architecture diagram, error bars, and complete timing table.
-   Checked local Markdown links and Pandoc-rendered documentation structure.
    Browser visual review remains unavailable because local file URLs are
    blocked; no workaround was used. `git diff --check` and added-file
    whitespace/private-path checks passed.

The selected run's ascending versus fixed-shuffle insertion medians at 4,096
levels are 4,359.6 ns and 105.7 ns, a ratio of medians of 41.2. Existing-level
C++ add-cancel pairs measured 44.4 ns at 4,096 levels. These figures describe
this run and workload; they are neither a speedup from the review changes nor
an estimate of per-call, cold-cache, Python, competitor, or exchange latency.

The original How to HFT site and archive were unavailable in the earlier
source review; attribution uses the repository's linked transcription.
Levine's release page was accessible. Historical engines were inspected as
source material, not executed. Temporary captures, review probes, and
rendered pages were removed after final evidence selection and verification.

## Shared Citation Update

Completed October 18, 2026 at the author's request. The report and software
now use the existing top-level `CITATION.bib` entry, without changing its key,
author, title, year, or URL. The separate report entry was removed. The
report's title and PDF metadata match the canonical title, while its 2026
date is explicitly a report revision. Both READMEs and `CITATION.cff` direct
readers to the same work. The seven prior-work references remain in
`docs/whitepaper/references.bib`, with the original provenance qualifications.

Validation:

-   `make -C docs/whitepaper clean` followed by `make -C docs/whitepaper`
    built eight pages with no final LaTeX or BibTeX warnings. BibTeX's log
    confirms it reads `../../CITATION.bib` and `references.bib` directly.
    The Makefile keeps BibTeX in the source directory to resolve that path.
-   The desktop LaTeX compiler also succeeded with the repository root as
    the project and `docs/whitepaper/main.tex` as its main document.
-   All eight rendered pages were reviewed. The canonical BibTeX is unchanged,
    its README copy is identical, and this update changes none of the 22
    hashed evaluation inputs. The PDF and CFF titles also match the BibTeX.
-   Relative documentation links and Pandoc-rendered Markdown structure were
    checked. `git diff --check` passed. No library code or study data changed,
    so native tests and measurements were not rerun for this citation update.

The report workflow also watches top-level `CITATION.bib` changes so a future
canonical citation edit triggers the branch PDF build.
