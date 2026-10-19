# Technical Report Builds

The report source and archived evaluation live in `docs/whitepaper/`. Its
PDF and LaTeX auxiliary files belong in `docs/whitepaper/.build/`, alongside
the manuscript. The C++ evaluation executable and shared library continue
to use the repository's root `.build/report/`.

## Local Build

From the repository root, run:

```shell
make -C docs/whitepaper
```

The output is `docs/whitepaper/.build/limit-order-book.pdf`. The Makefile is
the shared build entry point for local development and GitHub Actions. It
uses `latexmk`, `pdflatex`, and `bibtex`, disables shell escape, and reads
checked-in evidence without rerunning benchmarks or downloading research.
The Makefile adds the repository root to `BIBINPUTS` so BibTeX finds the
canonical `CITATION.bib` while running inside `.build/`. This also works with
TeX's restricted output settings on Ubuntu, without changing those settings.

On Ubuntu 24.04, the workflow installs these packages:

```shell
sudo apt-get install --no-install-recommends -y \
    latexmk texlive-latex-extra texlive-fonts-extra texlive-pictures \
    texlive-plain-generic
```

The `texlive-plain-generic` package supplies `binhex.tex`, required by the
Ubuntu version of `newtxtext`. Keep the dependency list consistent with the
manuscript's preamble. Do not
commit PDFs or auxiliary files: `.build/` is ignored at every depth.
`make -C docs/whitepaper clean` removes report outputs; `./main.sh clean`
removes the report directory and the root build directory.

## Shared Project Citation

The software and report are one cited work. The top-level
[CITATION.bib](../../CITATION.bib) owns the `kauten-limit-order-book-2019` entry,
including its title, author, 2019 citation year, and repository URL. The report reads
that entry directly through BibTeX; its
[references.bib](../whitepaper/references.bib) contains only prior work.
Do not create a separate report citation or copy the project entry there.

Keep the report title and PDF metadata, both READMEs, and
[CITATION.cff](../../CITATION.cff) aligned with that entry. A report revision
date identifies the document's state and does not replace the project citation
year. Preserve the evaluated source revision and evidence manifests when
updating presentation or citation metadata.

## Design Context And Historical Evidence

Keep the report centered on implementation choices and their consequences.
Historical comparisons need dated primary sources or an identified Git
artifact; distinguish source observations from measured comparisons. Do not
assign later bindings, packaging, or dependency versions to the original
design.

When components move or vendored dependencies change, update the construction
account and source map without rewriting archived paths, hashes, or timings.
Identify the inspected current revision separately from the measured one.
Verify replay instructions against every archived input, including the study
driver: copying a current driver onto old sources can silently change its
inputs or break its include paths. Record new measurements as a new study.

## Release Workflow

[report.yml](../../.github/workflows/report.yml) builds the checked-out report
and uploads only its PDF as the `limit-order-book-report` Actions artifact.
The upload explicitly includes hidden paths because the PDF is under `.build`.
An absent PDF fails the build instead of leaving an empty artifact.

[ci.yml](../../.github/workflows/ci.yml) calls this reusable workflow for pull
requests, `master` pushes, and tags. Its release job waits for the report and
package checks, downloads the report alongside the distributions, and uploads
all of them to the tag's GitHub release. Report-related pushes on other
branches run the report workflow directly, without creating releases.

A manually published release also triggers the report workflow. It checks
out `release.tag_name`, builds that tag's manuscript, and attaches the PDF
to that release. The tag must contain the report sources and Makefile.
Reruns replace the same PDF asset using `gh release upload --clobber`.
Manual workflow dispatch builds a preview artifact without publishing it.

Keep both release paths: events caused by `GITHUB_TOKEN` generally do not
start another workflow, so tag CI must upload its own PDF rather than rely
on the published-release trigger. See GitHub's
[workflow trigger documentation](https://docs.github.com/en/actions/how-tos/write-workflows/choose-when-workflows-run/trigger-a-workflow).
Only release upload jobs use write access; the report build uses read access.
The caller permits the reusable workflow's release-upload permission, but
that job is skipped for CI, branch pushes, and manual preview runs.

## Validation

Build locally, check the LaTeX log and rendered PDF, and run `actionlint` on
both workflows. Check the report-only Actions run after pushing. Validate the
upload wiring without creating a public release solely as a test.
