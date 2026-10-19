# Vendored Dependencies

`robin-map` supplies the price-to-limit hash map. Keep it committed in-tree so
header copying, CMake consumers, and Python source builds need no dependency
download for this library. Normal builds do not run the maintenance tool.

## Snapshot And Ownership

[vendor/robin-map.json](../../vendor/robin-map.json) records the upstream URL,
release, full Git commit, and SHA-256 of each retained file. The snapshot is
v1.3.0 at `188c45569cc2a5dd768077c193830b51d33a5020`, with no local patches.

Keep only `LICENSE` and all four `include/tsl/*.h` headers under
`vendor/robin-map/`. Retain `robin_set.h` even though the book uses only the
map, preserving the installed header set. Upstream tests, documentation,
build files, CI configuration, and debugger visualizers are not distributed.
The import tool never executes upstream build scripts or reads modified
working-tree files; it reads committed Git blobs.

The `.gitattributes` rule disables text conversion for this snapshot so Git
preserves its checksums on Windows, including with `core.autocrlf=true`.

Treat these files as upstream-owned. Make intentional updates through
[scripts/update_robin_map.py](../../scripts/update_robin_map.py), preserving
upstream bytes and license notices. Record any necessary patch explicitly
instead of silently changing the snapshot or its checksums.

The project requires C++14. Upstream [v1.3.0][release] supports C++11;
[v1.4.0][next-release] raises the requirement to C++17. Do not upgrade across
that boundary without an explicit project compatibility decision. A version
change also changes a type exposed through `LOB::PriceLimitMap`, so consumers
must rebuild code using the headers together with the matching snapshot.

## Verify Or Reproduce

The integrity check uses Python's standard library, with no Git or network
requirement. It rejects missing, altered, extra, or symlinked files:

```shell
python3 scripts/update_robin_map.py --check
```

To recreate the recorded snapshot, clone upstream into a temporary directory
outside the checkout, then import from the manifest. Git and network access
are needed only to obtain the clone; an existing clone containing the pinned
commit also works offline:

```shell
scratch=$(mktemp -d)
git clone --no-checkout https://github.com/Tessil/robin-map.git "$scratch/robin-map"
python3 scripts/update_robin_map.py --source "$scratch/robin-map"
python3 scripts/update_robin_map.py --check
git diff --exit-code -- vendor/robin-map vendor/robin-map.json
rm -rf "$scratch"
```

The final diff assumes the snapshot and manifest were already committed.
The tool validates the version and every source checksum before changing
vendored files. It removes files outside the retained set only inside
`vendor/robin-map/`. It does not fetch, commit, or publish anything.

## Update A Pin

1.  Read the upstream release notes and check the required C++ standard, API,
    license, and compiler support. Resolve the release tag to a full commit
    with `git rev-parse 'v1.3.0^{commit}'` in the upstream clone. Use a peeled
    commit, not an annotated tag object's ID.
2.  Capture baseline tests and repeated benchmarks before changing the pin.
    Keep the captures and upstream clone outside this checkout.
3.  Pass both the new version and full commit explicitly. For this snapshot:

    ```shell
    python3 scripts/update_robin_map.py --source "$scratch/robin-map" --version 1.3.0 --commit 188c45569cc2a5dd768077c193830b51d33a5020
    ```

    This records new hashes rather than checking against the previous pin.
    Review the manifest and header diff against the intended upstream source.
    Repeating the import without `--version` and `--commit` must reproduce
    identical files. Do not change only the checksums to hide local edits.
4.  Run the validation below, inspect package contents, update this guide and
    the changelog, and record the decision and evidence in the owning spec.
    CI runs the offline integrity check alongside Python package validation.
5.  Remove temporary clones and captures after recording the results.

## Validation

```shell
python3 scripts/update_robin_map.py --check
./main.sh test
python3 -m unittest discover -s scripts -p 'test_*.py'
./main.sh test-package
./main.sh benchmark '~[bst-benchmark]' --benchmark-samples 20 --benchmark-resamples 1000
./main.sh benchmark-python --loops 100000 --repeats 9 --output /tmp/lob-robin-after.json
./main.sh deployment
git diff --check
```

Use the same compiler, flags, source state, workloads, and repeat counts for
both versions. Repeat in alternating order where possible. The
[report evaluation driver](../whitepaper/evaluate.py) additionally checks
seeded event streams against an independent reference book and measures C++
and C operations. Do not overwrite the report's archived evidence.

Check that installed packages contain the four headers and license, the sdist
contains the manifest and trimmed snapshot, and wheels contain the license.
Run the installed wheel suite with `python -I test/package/python.py`. Local
macOS results do not establish Linux or Windows compatibility; the CI matrix
owns those checks. Do not claim a speedup from overlapping timing ranges.

[release]: https://github.com/Tessil/robin-map/tree/v1.3.0
[next-release]: https://github.com/Tessil/robin-map/releases/tag/v1.4.0
