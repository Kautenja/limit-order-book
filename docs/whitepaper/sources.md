# Report Sources And Claim Boundaries

Reviewed October 19, 2026. The report explains construction and design
tradeoffs using historical sources, the original Git artifact, and current
implementation code. It keeps those claims distinct from generated
correctness checks and measurements of an earlier snapshot.

## Historical Frame

The [initial commit][initial], `c0dfd34ae6f38368f8089a47f5e670686ecf80b0`,
has author date December 16, 2019. It contains:

-   `notes/lob.md`, the How to HFT transcription.
-   `notes/engines/engine.c` and `notes/engines/island.prg`, the historical
    engine sources now under `docs/references/engines/`.
-   `include/limit_order_book/structures.hpp`, with `Order : DLL::Node` and
    `Limit : BST::Node<Price>`.
-   `include/limit_order_book/limit_tree.hpp`, with the price tree, cached best,
    and `tsl::robin_map<Price, Limit*>`.
-   `include/limit_order_book/limit_order_book.hpp`, with the two sides and
    `std::unordered_map<UID, Order>`.
-   The tree, list, and robin-map headers under their original vendor paths.

This establishes that the examples and central representation were present
when the repository began. It does not date the undated QuantCup source or
claim that either example engine was copied into the library. The initial
API included account and timestamp fields and assigned IDs internally;
the report's behavioral model describes the later implementation. C++14
packaging, current Cython bindings, helper integration, and the pinned
robin-map v1.3.0 snapshot are not presented as original 2019 features.

The historical comparison is an engineering interpretation of inspected
source: sparse occupied-price storage avoids reserving the numeric price
range; intrusive predecessors enable direct cancellation; arena allocation
and record reuse offer different storage tradeoffs. It is not an empirical
ranking or a claim that one design was the industry-wide best choice.

[initial]: https://github.com/Kautenja/limit-order-book/tree/c0dfd34ae6f38368f8089a47f5e670686ecf80b0

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
    [Upstream v1.3.0 documentation](https://github.com/Tessil/robin-map/tree/v1.3.0)
    was read to verify attribution, Robin Hood/backward-shift behavior, and
    invalidation of references to table entries. The current
    [vendor manifest](../../vendor/robin-map.json) pins upstream commit
    `188c45569cc2a5dd768077c193830b51d33a5020`. The book stores pointers in this
    map; separately allocated limits retain their addresses when table entries
    move. Runtime results in the paper concern the older hashed headers in
    the study archive, not v1.3.0 or the latest upstream code.
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

The current implementation account was checked against `83b6590`, after the
helper integration and pinned robin-map update. That revision is distinct
from the archived measurement inputs below.

-   [binary_search_tree.hpp](../../include/binary_search_tree.hpp): Borrowed
    parent/child links, unbalanced insertion, successor-based removal, and
    min/max walks. Local ownership does not change these algorithms.
-   [doubly_linked_list.hpp](../../include/doubly_linked_list.hpp): Embedded
    predecessor/successor links, append through a tail, and direct unlinking.
    The book does not use the helper that traverses from a head to append.
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
The archive predates the move of the tree and list into `include/` and the
robin-map v1.3.0 update on October 19, 2026. Its original vendor paths and
hashes remain unchanged; the [ownership guide](../development/data-structures.md)
and [vendoring guide](../development/vendored-dependencies.md) describe the
current components. All 22 entries in the three source manifests match Git
commit `336b2168811290b8d67a2c367ff19f946a183cdc`, which includes the report's
original driver. The [replay instructions](README.md#inspect-and-regenerate-evidence)
use that commit so current include-path changes do not break a replay.
The insertion effect is a ratio of medians, not a median of paired ratios or
an isolated estimate of tree-traversal cost. The study does not measure Python
overhead, network latency, request-tail
latency, real market traces, memory consumption, or competing engines.

## Presentation References

The report adopts the two-column technical-report treatment used by
[RackNES](https://github.com/Arhythmetic-Units/RackNES/tree/master/whitepaper)
and [Fourier](https://github.com/Arhythmetic-Units/Fourier/tree/main/docs/whitepaper).
Their subject matter and empirical results are not evidence about this book.
