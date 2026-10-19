# Markdown Style Guide

This guide is for agents and humans writing or editing Markdown in this
repository: `README.md`, `AGENTS.md`, the developer manual, and specs.
[AGENTS.md](../AGENTS.md) owns workflow and completion requirements; this file
owns Markdown presentation.

The style follows Christian Kauten's public repositories, including this
project's own `README.md`: a direct opening paragraph, practical sections,
fenced commands, `-   ` bullets, and reference-style badge links. Apply these
rules to new and edited sections, and avoid unrelated style churn.

## File Organization

Place Markdown by audience and lifetime:

-   **`README.md`:** Public orientation, rendered on GitHub, on PyPI through
    `pyproject.toml`, and as the Doxygen main page. It covers what the project
    is, installation, usage, and development commands.
-   **`AGENTS.md`:** The entry point for agents: workflow, architecture,
    contracts, and validation.
-   **`developer-manual/`:** Durable rules for agents and maintainers, such as
    style guides, testing methodology, and architecture notes.
-   **`specs/`:** Dated, testable work definitions with completion evidence,
    following [AGENTS.md](../AGENTS.md#planning-and-completion).
-   **`notes/`:** Imported research and quoted source material. Preserve its
    original formatting and punctuation; do not restyle it.
-   **`docs/`:** Generated Doxygen HTML for GitHub Pages. Never add Markdown
    there.

`Doxyfile` scans `*.md` files recursively. When adding a Markdown directory
that should not appear in the API reference, add it to `EXCLUDE` in
`Doxyfile`.

## Structure And Prose

-   Use one `#` title and ATX headings (`##`, `###`) in Title Case. Preserve
    the spelling of code identifiers and product names. Do not skip levels.
-   Start with one or two short paragraphs naming the document's purpose. Use
    concrete headings instead of Overview, Details, or Conclusion.
-   Leave one blank line between headings, paragraphs, lists, tables, code
    fences, blockquotes, and HTML blocks.
-   Wrap prose and list items near 80 columns. Do not break URLs, inline code,
    table rows, or copyable commands to satisfy the line length.
-   Use direct, practical language with exact names, types, and constraints.
    Avoid slogans, inflated claims, and implementation diaries.
-   Default to ASCII punctuation in new prose: straight quotes, `-` instead
    of en or em dashes, and `...` instead of the ellipsis character.
-   Use `$O(1)$` style inline math for complexity, as `notes/lob.md` does.
-   Link to [LICENSE](../LICENSE) instead of duplicating license text.
    Markdown files do not get the source-code copyright header.
-   Do not use emoji.

## Lists And Emphasis

-   Use `-   ` for ordinary bullets and four spaces for continuation lines.
    Nested bullets start with `    -   `; use nesting sparingly.
-   Use `1.  `, `2.  `, and later numbers with two spaces after the period.
    Use ordered lists only when sequence or priority matters.
-   Use `- [ ]` and `- [x]` for GitHub task lists. A checkbox does not replace
    evidence that a check ran.
-   Keep items parallel in grammar and punctuation. End items with periods
    when most are complete sentences.
-   Use a bold label and colon for named items, such as layout maps and
    feature lists. Reserve backticks for literal paths, commands,
    identifiers, flags, and values.
-   Use `**Note:**` for helpful context, `**Important:**` for a rule a future
    agent could break, and `**Warning:**` for destructive commands. Use
    blockquotes only for quoted source material.

For example:

```markdown
-   **`include/`:** The header-only C++ library.
-   **`limit_order_book/`:** The Python package and its `ctypes` bindings.

1.  Build the shared library.
2.  Run the Python suite.
```

## Tables

Use tables for compact comparisons, such as supported platforms, type
mappings, and benchmark results. Use labeled bullets or prose when each row
needs a paragraph of explanation. Tables must read cleanly as plain text in
a monospaced editor and render as ordinary GitHub tables.

1.  Use a leading and trailing `|` on every row, and spaces for padding.
2.  Align every column boundary vertically across the header, separator, and
    data rows. Size each column for its longest cell, including markup.
3.  Leave at least one space between each cell and its pipes. Left-align
    text. Right-align numbers in the source and mark the separator with a
    trailing colon. Put shared units in the column header.
4.  Extend separator cells to the full column width.
5.  Keep one physical line per row and the same number of cells in each row.
    Escape literal pipes as `\|`.
6.  Keep table lines within 100 characters. Shorten cells, use
    reference-style links, or move explanations below the table.
7.  After changing any cell, realign the whole table.

Text columns:

```markdown
| C++ Alias  | Width     | `ctypes` Type     |
| ---------- | --------- | ----------------- |
| `UID`      | 64 bits   | `ctypes.c_uint64` |
| `Quantity` | 32 bits   | `ctypes.c_uint32` |
```

Numeric columns:

```markdown
| Limits | Median (ns) |
| -----: | ----------: |
|      1 |          42 |
|   1000 |       39100 |
```

## Code And Commands

-   Fence code with three backticks and a language tag: `cpp`, `c`, `python`,
    `shell`, `text`, or `markdown`. Use a longer outer fence when an example
    contains fenced code.
-   Use `shell` for copyable commands, matching `README.md`. Use `sh` for POSIX
    snippets or heredocs and `bash` only for Bash-specific syntax. Use `text`
    for output and logs.
-   Omit `$` prompt markers from copyable commands.
-   State the working directory and prerequisites when they are not the
    repository root and the tools in `README.md`.
-   Check commands against `makefile`, `SConstruct`, `pyproject.toml`, and
    `.github/workflows/` before documenting them. Do not import commands from
    other projects.
-   Put the command before its results in evidence sections.

## Links And Media

-   Use relative links for repository files. Link the first useful mention,
    not every mention.
-   Use reference-style links for badges, repeated targets, and long link
    sets. Keep badge definitions directly below the badge row, as `README.md`
    does.
-   Use descriptive link text, never a bare "here".
-   Keep heading text stable in `AGENTS.md` and the developer manual, because
    other documents link to their anchors. Update incoming links when a
    heading changes.
-   Give images useful alt text and prefer checked-in assets under
    `doxygen/img/`. Use HTML only when Markdown cannot express the layout.

## Evidence And Release Notes

-   Record exact commands, the host and architecture, compiler version, build
    flags, workload size, and repeated runs for benchmark evidence. Put
    results in a table with units, then state what the result proves.
-   Separate builds, test runs, and performance claims. A noisy or
    inconclusive result is reported as noisy or inconclusive.
-   Use `# Changelog` and version headings such as `## 2.1.0 (2026-10-18)` if
    a changelog is added. Use past tense and call out breaking changes to the
    C++, C, or Python interfaces explicitly.

## Review Before Committing

Before finishing a Markdown change, check:

-   The document is in the right directory for its audience.
-   The first paragraph says what the document is for.
-   Headings, lists, fences, and tables follow this guide in the source view.
-   Commands match the current `makefile`, `SConstruct`, `pyproject.toml`,
    and `.github/workflows/`.
-   Relative links and heading anchors resolve.
-   The rendered Markdown looks right on GitHub.
-   `git diff --check` passes. It checks whitespace only, not table alignment
    or rendering.
