# limit-order-book Licensing

The repository's declared software license is the MIT License. The standard
text and Christian Kauten's copyright notice are in [LICENSE](LICENSE). This
guide keeps scope, attribution, and third-party information separate from that
text. It does not change the license terms.

## License Detection And Package Metadata

Keep the root `LICENSE` limited to the standard license text and copyright
notice. GitHub's [license detection guidance][github-license] recommends
putting scope explanations elsewhere so its detector can match the standard
text. Put project-specific explanations in this file.

[pyproject.toml](pyproject.toml) and [CITATION.cff](CITATION.cff) declare the
SPDX identifier `MIT`. The `license-files` setting includes both `LICENSE` and
this guide in source and wheel distributions. License detection reports the
repository's declaration; it does not establish the licensing history of every
file in the repository.

## Original Code And Documentation

Copyright (c) 2019 Christian Kauten. The MIT License applies to the original
C++ headers in `include/`, the C API in `src/`, the Python package in
`limit_order_book/`, the tests, benchmarks, executables in `main/`, build and
workflow configuration, and the documentation. The C++ sources carry the MIT notice in their headers; preserve
those notices when copying `include/` into another project.

## Vendored Dependencies

The libraries in `vendor/` are committed in-tree and retain their own licenses.
The root license does not replace their terms. Keep each library's license file
with its code when copying or redistributing it.

| Library              | Use                                | License                      |
| -------------------- | ---------------------------------- | ---------------------------- |
| `binary-search-tree` | Limit tree nodes                   | MIT, Christian Kauten        |
| `doubly-linked-list` | Order queue at each limit          | MIT, Christian Kauten        |
| `robin-map`          | Price-to-limit hash map            | MIT, Tessil                  |
| `hopscotch_map`      | Alternative hash map, not included | MIT, Tessil                  |
| `sparse-map`         | Alternative hash map, not included | MIT, Tessil                  |
| `cpptqdm`            | Progress bars, not included        | MIT, Christian Kauten et al. |

`binary-search-tree`, `doubly-linked-list`, and `robin-map` are included by
the public headers. `cmake --install` installs their headers beside the
project headers and their license files under
`share/doc/limit_order_book/licenses/`, and they are compiled into the C API
shared library, so wheels contain portions of them. Their MIT terms ask that
their copyright and permission notices accompany substantial portions of the
software. The other libraries are not included by any first-party source and
are not installed.

The source distribution includes the three used libraries with their license
files and excludes the unused libraries and the nested `vendor/*/vendor`,
test, and benchmark directories, as declared by `sdist.exclude` in
[pyproject.toml](pyproject.toml).

Catch2, under the Boost Software License 1.0, is not vendored. CMake fetches
it from GitHub only to build the test and benchmark executables, and it is not
part of any distribution or installation.

## Notes And Example Engines

The material in `docs/references/` is imported reference material, not
original project code, and is not licensed under MIT by this project:

-   **`docs/references/lob.md`:** Quoted posts from the archived "How to HFT"
    blog, with a link to the source. Copyright remains with the original author.
-   **`docs/references/engines/engine.c`:** A QuantCup 1 price-time matching
    engine submission credited to its author, "voyager".
-   **`docs/references/engines/island.prg`:** Source attributed to the Island ECN
    matching engine.

These files carry no license grant from their authors in this repository.
Their presence does not grant permission to copy, modify, or redistribute them.
They are excluded from the Python distributions. Do not
copy their code into `include/`, `src/`, or `limit_order_book/`; describe and
link ideas from them instead.

## Maintaining This Layout

Keep `LICENSE`, the declared SPDX identifiers, and this guide consistent with
any intentional licensing decision. Keep explanatory prose out of `LICENSE`,
preserve existing notices, and update the vendored dependency table when a
library is added, removed, or relicensed upstream. After packaging changes,
verify that both distributions include the license files. Follow
[CONTRIBUTING.md](CONTRIBUTING.md) for the build and contribution workflow.

[github-license]: https://docs.github.com/en/repositories/managing-your-repositorys-settings-and-features/customizing-your-repository/licensing-a-repository#detecting-a-license
