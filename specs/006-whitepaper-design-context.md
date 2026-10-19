# Whitepaper Design And Historical Context

Created: 2026-10-19
Status: COMPLETE

## Goal

Explain how the book was built and why its choices are useful in the context
of the design literature and example engines available when the project
began. Incorporate the now-owned tree/list code and pinned vendored hash map,
then commit and push the report changes.

## Scope

-   Lead with the implementation, its historical context, and its tradeoffs.
-   Explain the intrusive structures, distinct hash-map roles, cached state,
    and common update paths using the inspected source.
-   Separate the 2019 design, 2026 maintenance, and archived measured revision.
-   Preserve the study and move detailed protocol material to an appendix.
-   Update the source map, bibliography, and report reproduction instructions.

## Non-Goals

-   Change matching behavior, bindings, dependencies, or build infrastructure.
-   Replace historical measurements or claim a new comparative speedup.
-   Commit existing changes outside this report task, create a release, or
    publish packages.

## Acceptance Criteria

-   Every historical claim has a primary source or an identified Git artifact.
-   The paper explains benefits and costs of the actual implementation without
    assigning later dependencies, bindings, or packaging to the original design.
-   The archived evidence remains byte-identical and reproducible; the replay
    recipe restores the evaluated driver rather than overlaying a newer driver.
-   The PDF builds with resolved citations and references, no layout warnings,
    and all pages visually reviewed.
-   Documentation links, rendered Markdown, and whitespace checks pass.
-   Only the report task's changes are committed and pushed on the current branch.

## Validation Commands

Run from the repository root:

```shell
python3 docs/whitepaper/evaluate.py --summarize docs/whitepaper/evidence
git diff --exit-code -- docs/whitepaper/evidence
make -C docs/whitepaper
pdfinfo docs/whitepaper/.build/limit-order-book.pdf
git diff --check
```

Verify the replay recipe and evidence hashes in a temporary directory outside
the checkout. Render all PDF pages and the changed Markdown for review, then
remove temporary captures. Inspect the report Actions run after pushing.

## Completion Evidence

Completed on 2026-10-19, with local validation on macOS arm64.

-   Reframed the abstract, introduction, construction account, and conclusion
    around the design choices and their benefits. Added a source-based
    comparison of How to HFT, QuantCup, Island2, and this library. The detailed
    evaluation protocol now accompanies the complete timing table in appendices.
-   Inspected the first commit, `c0dfd34`, to establish that the reference
    sources and tree/queue/hash representation were already present in
    December 2019. Current implementation claims were checked at `83b6590`,
    after the helper and robin-map changes were committed separately.
-   Distinguished the first-party helpers and current v1.3.0 pin from the older
    measured snapshot. Updated the bibliography and source map, and documented
    this distinction in the owning report-build guide.
-   `python3 docs/whitepaper/evaluate.py --summarize docs/whitepaper/evidence`
    regenerated every numerical input byte for byte.
    `git diff --exit-code -- docs/whitepaper/evidence` passed; no raw data,
    measurement metadata, or generated result changed.
-   Verified all 22 source-manifest entries against original report commit
    `336b2168811290b8d67a2c367ff19f946a183cdc`. Tested the README's exact
    `git worktree add --detach /tmp/lob-report-replay` recipe at that revision,
    then ran `python3 docs/whitepaper/evaluate.py --output /tmp/lob-report-replay-results`
    inside it. The original driver compiled and completed 20 streams of
    10,000 selections and all 225 timing observations. Its 13 evaluated-source
    hashes and all seeded check records match the archive. New timings were
    used only to verify the recipe, not adopted as report evidence.
-   `python3 scripts/update_robin_map.py --check` verified the current pin's
    five files, supporting the component description without modifying them.
-   `make -C docs/whitepaper` built a nine-page PDF with resolved references
    and no LaTeX warnings or overfull/underfull boxes. All final pages were
    rendered with Poppler and visually reviewed, including the comparison
    table, architecture diagram, timing chart, bibliography, and appendices.
-   Checked 41 relative documentation paths. Pandoc rendered the changed
    Markdown to HTML and a temporary PDF; the rendered document structure
    and all seven review pages were inspected. `git diff --check` passed.

The original How to HFT site remained unavailable; the report uses the
attributed local transcription and records that limit. Levine's release page
and robin-map's tagged upstream documentation were accessible. The ordinary
C++/Python/package suites were not rerun for this documentation-only change.
`actionlint` was unavailable locally; workflow files were unchanged.
The replay is a macOS check, not Linux or Windows validation. Historical
engines were inspected, not benchmarked. Temporary replay and preview outputs
are removed after review; the final PDF remains in the ignored report build.

The report changes were committed and pushed as `b73f183` on
`2026-10-19-updates`. `gh run watch 39201142411 --interval 30 --exit-status`
confirmed that the Ubuntu 24.04 report build passed and uploaded its PDF:
[Actions run 39201142411](https://github.com/Kautenja/limit-order-book/actions/runs/39201142411).
The release-attachment job correctly skipped the branch push. This validates
the report build on Ubuntu, not the library's Linux behavior. The working tree
was clean after the push; this final evidence note is a separate documentation
commit.
