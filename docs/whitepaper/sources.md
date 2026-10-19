# Report Sources And Claim Boundaries

Reviewed October 18, 2026. The report separates inherited design, source
analysis, generated correctness checks, and measured timing results.

## Prior Work

-   **How to HFT, "How to Build a Fast Limit Order Book":** The direct design
    antecedent for per-side price trees, per-price FIFO queues, ID and price
    indices, and cached inside prices. The report cites the original article
    under its published handle `howtohft`. It does not guess a legal name or
    claim independent invention of that architecture.
    [Original article](http://howtohft.wordpress.com/2011/02/15/how-to-build-a-fast-limit-order-book/),
    [linked archive](https://web.archive.org/web/20110219155647/http://howtohft.wordpress.com/author/howtohft/),
    [repository transcription](../references/lob.md). The original site and
    archive could not be fetched during this review; the attributed local
    transcription was inspected. Its balancing qualification is discussed in
    the report. Imported prose and code are not reproduced in the manuscript.
-   **Christian Kauten, binary-search-tree:** The inspected header
    implements insertion, removal, and min/max traversal without balancing.
    It is now maintained as [first-party source](../../include/binary_search_tree.hpp).
    Tree-height complexity and the ordered-insertion sum are derived directly
    from this implementation, rather than attributed to a new algorithm.
-   **Christian Kauten, doubly-linked-list:** Intrusive order links and queue
    operations, now maintained as
    [first-party source](../../include/doubly_linked_list.hpp).
-   **Tessil and contributors, robin-map:** The price-to-level index.
    [Upstream documentation](https://github.com/Tessil/robin-map) was read to
    verify attribution and the Robin Hood/backward-shift description. Runtime
    claims concern the hashed vendored headers, not the latest upstream code.
-   **Cython developers, "Using C++ in Cython":**
    [Official user guide](https://docs.cython.org/en/latest/src/userguide/wrapping_CPlusPlus.html),
    consulted for the standard native wrapper and exception-declaration model.
    The project-specific behavior is established by `_book.pxd` and
    `_native.pyx`, not inferred from the user guide.

## Historical Engine Sources

Both files are imported reference implementations. They are explicitly cited
in the report, but are not performance baselines or first-party code. Their
presence does not establish direct code ancestry. The report does not copy
their code, infer a license grant, or adopt performance statements from their
comments. [reference-sources.json](evidence/reference-sources.json) records
SHA-256 hashes of the exact local bytes inspected, preserving the original
Island file's encoding and line endings.

-   **voyager, "QuantCup 1: Price-Time Matching Engine":**
    [Local source](../references/engines/engine.c), especially the author
    credit at line 4, `pricePoints` and `arenaBookEntries` declarations,
    `ppInsertOrder`, `limit`, and `cancel`. The array is indexed by numeric
    price; orders come from a fixed arena; queue insertion appends at the
    tail; cancellation sets size to zero without unlinking. Matching visits
    those entries later and skips zero-sized execution reports. Price-slot
    scans and deferred removal motivate the comparison with the library's
    tree and immediate unlinking. These are source observations, not measured
    superiority claims. The local file has no publication date; the report
    preserves the credited handle and does not assert a contest ranking.
-   **Joshua Levine, Island2, version 2.40:**
    [Local source](../references/engines/island.prg), with version at line 17
    and the 1996 author notice at line 392. The `enter2order` routine uses
    `SSEEK`/`BSEEK` database indices, handles quantity conditions, emits
    execution messages, and links depleted records into `tnextdead` for reuse.
    `new2cancel` checks a record locator and sequence before changing the
    remaining quantity. The database index definitions and support runtime
    are absent, so this file alone cannot certify the exact ordering policy
    or data-structure complexity.
    [Levine's original release page](https://www.josh.com/notes/island-ecn-10th-birthday/)
    describes a FoxPro snapshot dated December 15, 2003 and explains that
    indexing and messaging depend on other software. The page was inspected;
    its linked source failed to decode in the web tool, so byte identity with
    the local copy was not verified. The 1996 copyright year is not treated as
    a version date, and the local file is not called the first Island engine.

Attribution and licensing scope remain separate; see
[LICENSING.md](../../LICENSING.md).

## Implementation Evidence

-   [structures.hpp](../../include/structures.hpp): Integer widths, order and
    limit fields, intrusive inheritance, and count/volume storage.
-   [limit_tree.hpp](../../include/limit_tree.hpp): Queue updates, matching,
    price-zero sentinel, tree removal, best-price traversal, and explicit clear.
-   [limit_order_book.hpp](../../include/limit_order_book.hpp): Identifier
    uniqueness, crossing, reduction, market remainder behavior, aggregate
    queries, implicit copy/destruction behavior, and midpoint arithmetic.
-   [C header](../../include/limit_order_book_c.h) and
    [definitions](../../src/limit_order_book_c.cpp): Opaque handles, fixed-width
    types, unknown-ID checks, and absence of general exception containment.
-   [Cython declarations](../../limit_order_book/_book.pxd),
    [native wrapper](../../limit_order_book/_native.pyx), and
    [Python binding guide](../development/python-bindings.md): Current Python
    ownership, argument conversion, exception translation, and GIL behavior.
-   [CMakeLists.txt](../../CMakeLists.txt): Header-only default subproject,
    optional C and Python targets, C++14, and installed consumer interface.

## Evaluation Evidence

The independent vector oracle is newly written first-party study code. It
scans arrival-ordered values and does not call the implementation's matching
helpers. Comparisons after each generated event selection cover live order
fields, the ID set, price and side aggregates, best prices, and FIFO links and
endpoints. The model supplies the expected arrival order; the checker inspects
the implementation's public order links to compare it. Tree and hash internals
are not exhaustively checked. The 200,000 selections include no-ops, and the
archive records selection counts, duplicate limits, and empty updates. Its
finite positive input domain is documented; agreement is not formal
verification or a differential check of C/Python wrappers.

The timing study uses fresh books for insertion and sweeps, and restores the
anchor state after every existing-level pair. Repeated use of one ID and price
is a hot-path workload; a fresh logical book does not imply cold caches or
allocator state. The study keeps setup, validation, and
explicit residual-level cleanup outside timing. The dynamic C bridge is
compiled separately without LTO. The reference model is not timed or advertised
as a competitive baseline.

All timing claims derive from `evidence/timings.csv`. `summary.json` and the
LaTeX/plot inputs are generated by `evaluate.py`. Hardware and toolchain claims
derive from `metadata.json`; repository-suite results are in `validation.json`.
The archive predates the move of the tree and list into `include/` on
October 19, 2026. Its original vendor paths and hashes remain unchanged;
the [ownership guide](../development/data-structures.md) explains how to
interpret those records alongside the current source.
The insertion effect is a ratio of medians, not a median of paired ratios or
an isolated estimate of tree-traversal cost. The study does not measure Python
overhead, network latency, request-tail
latency, real market traces, memory consumption, or competing engines.

## Presentation References

The report adopts the two-column technical-report treatment used by
[RackNES](https://github.com/Arhythmetic-Units/RackNES/tree/master/whitepaper)
and [Fourier](https://github.com/Arhythmetic-Units/Fourier/tree/main/docs/whitepaper).
Their subject matter and empirical results are not evidence about this book.
