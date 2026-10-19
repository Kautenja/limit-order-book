# Documentation

This directory brings together development guidance and imported reference
material for `limit-order-book`. Start with the project [README](../README.md)
for installation, usage, and development commands.

## Technical Report

-   [Design and evaluation](whitepaper/README.md): LaTeX manuscript, attribution,
    matching semantics, complexity, and reproducible correctness and timing
    experiments with archived observations.

## Development

-   [Contributing](../CONTRIBUTING.md): Environment setup, architecture,
    testing, releases, and the pull request workflow.
-   [Agent instructions](../AGENTS.md): Repository layout, matching and API
    contracts, and validation requirements.
-   [C++ Style Guide](development/style-guide-cpp.md): Conventions for the
    library, C API, tests, and benchmarks.
-   [Data structures](development/data-structures.md): Local ownership,
    compatibility, and validation of the intrusive tree and list.
-   [Python bindings](development/python-bindings.md): Cython ownership,
    conversions, builds, and repeatable performance measurements.
-   [Report builds](development/technical-report.md): Local PDF output,
    LaTeX dependencies, and GitHub release attachments.
-   [Markdown Style Guide](development/style-guide-markdown.md): Document
    organization, prose, formatting, links, and review checks.
-   [Changelog](../CHANGELOG.md): Release history and compatibility changes.
-   [Licensing](../LICENSING.md): License scope, attribution, and dependencies.

## Reference Material

These imported sources provide background on matching engine design. They
are reference material, not project guidance. Their original content is
preserved; see the [licensing guide](../LICENSING.md#notes-and-example-engines)
for their attribution and redistribution restrictions.

-   [Limit order book research](references/lob.md): Archived "How to HFT"
    posts about trading systems, order books, latency, and message queues.
-   [QuantCup engine](references/engines/engine.c): The price-time matching
    engine submission credited to "voyager".
-   [Island ECN engine](references/engines/island.prg): Historical source
    attributed to the Island ECN matching engine.
