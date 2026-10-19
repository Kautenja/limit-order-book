# Report Build And Release Artifact

Created: 2026-10-18
Status: COMPLETE

## Goal

Build the report beside its source in `docs/whitepaper/.build/`, and attach
the compiled PDF to GitHub releases. Commit and push the change.

## Scope

-   Move LaTeX outputs and clean behavior to the report's local `.build/`.
-   Reuse one report build workflow from tag CI and published-release events.
-   Make tag releases depend on the PDF build and attach the PDF with packages.
-   Document output paths, release behavior, and maintenance requirements.

## Non-Goals

-   Change the manuscript, study protocol, measured data, or package APIs.
-   Move the evaluation executable or shared library from root `.build/report/`.
-   Create a release or tag, or publish packages while validating this change.

## Acceptance Criteria

-   `make -C docs/whitepaper` creates a nonempty eight-page PDF at
    `docs/whitepaper/.build/limit-order-book.pdf` with no LaTeX warnings.
-   The local output is ignored by Git and removed by report and project clean.
-   Tag CI waits for the report and uploads its PDF to the matching release.
-   A manually published release builds its tag's report and uploads the PDF.
-   Report-only branch pushes can validate the Ubuntu build without publishing.
-   Workflow syntax, package checks, and the unchanged evidence manifests pass.
-   Changes are committed and pushed to the current branch.

## Validation Commands

Run from the repository root:

```shell
make -C docs/whitepaper clean
make -C docs/whitepaper
git check-ignore docs/whitepaper/.build/limit-order-book.pdf
bash -n main.sh
actionlint .github/workflows/ci.yml .github/workflows/report.yml
./main.sh test
./main.sh test-package
./main.sh deployment
make -C docs/whitepaper
git diff --check
```

Render and inspect all eight pages of the final PDF.
Inspect the report workflow after pushing; do not create a release to test
the upload. Record actual results and remaining limitations below.

## Completion Evidence

Local validation on 2026-10-18, macOS arm64:

-   `make -C docs/whitepaper clean` removed the PDF; the next build recreated
    it at the requested path. The final LaTeX pass had no warnings or
    overfull/underfull boxes. All eight rendered pages were inspected.
-   `git check-ignore` confirmed that the new output path is ignored.
    `./main.sh deployment` exercised project clean and removed the report's
    build directory; a final `make -C docs/whitepaper` restored the PDF.
-   `actionlint` 1.7.12 passed for both workflows. The official release
    archive's SHA-256 was checked before running the tool.
-   `bash -n main.sh` and `git diff --check` passed. Relative documentation
    file links resolved, and the new guide's rendered HTML was reviewed.
-   `./main.sh test` passed all 66 C++ cases and 28 Python cases.
-   `./main.sh test-package` passed installed-package and FetchContent
    consumer checks for C++ and C.
-   `./main.sh deployment` built the source distribution and macOS arm64
    CPython 3.14 wheel. Its first attempt could not download dependencies
    under the network sandbox; rerunning with network access passed.
-   All 22 entries in the study, architecture, and historical-reference
    source manifests still match. The manuscript and evaluation are unchanged.

The first Ubuntu Actions run found a missing `binhex.tex` dependency in
`newtxtext`. The workflow and guide now explicitly include
`texlive-plain-generic`; `actionlint` passes after this correction.

The corrected workflow passed on Ubuntu 24.04 at commit `7dcddf2`:
[Actions run 39195349031](https://github.com/Kautenja/limit-order-book/actions/runs/39195349031).
The downloaded `limit-order-book-report` artifact contains an eight-page PDF;
the final LaTeX pass has no unresolved warnings or overfull/underfull boxes.
All eight pages match the local PDF's normalized extracted text.
The release-upload job correctly skipped the branch-push event. The build
and dependency changes were committed and pushed to `2026-10-18-updates`.

The release paths were inspected without creating a tag, release, or PyPI
publication. Actual release attachment remains unexercised until a release.
