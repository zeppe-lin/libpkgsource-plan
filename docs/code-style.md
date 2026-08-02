# Code style

## Purpose

The repository style exists to make authority transfer, protocol framing, and
ownership movement reviewable. Formatting is automated; semantic clarity is a
review obligation.

## C++ formatting

`clang-format 17` and the repository `.clang-format` are canonical. CI runs the
formatter in dry-run error mode over installed headers, implementation sources,
and C++ tests.

Control statements always use braces, including one-statement bodies. Braces
must not be omitted to save vertical space. A later comment or statement must
not be able to change control flow by indentation alone.

Includes are grouped in this order:

1. the corresponding public or private project header;
2. other private project headers;
3. owner-library headers;
4. provider headers, only inside provider implementations;
5. standard-library headers.

## C++ design

Public headers expose owner types and stable adapter types only. Provider types,
implementation helpers, and protocol writers remain under `src/internal`.

A translation unit should have one dominant responsibility. `adapter.cpp`
coordinates projection. Candidate-control framing, source digest import, and
SHA-256 execution belong to separate internal units.

Compute identities before moving their source values. Never depend on function
argument evaluation order or the contents of moved-from objects.

Protocol constants use named values. Enum representations are never serialized
implicitly. Canonical field order is documented beside the code that enforces
it and in the normative protocol document.

Catch only exceptions whose ownership is part of the boundary. Do not convert
`std::exception` wholesale into a more specific domain error.

## Comments and Doxygen

Installed headers carry complete Doxygen for public types, parameters, return
values, ownership, and exceptions. Public Doxygen is the API source of truth.

Implementation comments explain facts that are not obvious from syntax:
canonical ordering, owner normalization, protocol coupling, defensive branches,
move timing, and exception translation. They do not narrate individual
statements or repeat the public manual.

Private headers may use concise Doxygen-style comments for IDE navigation, but
they are not included in the published public API documentation.

## Tests

Behavioral tests are organized by contract, not by implementation file.
Projection tests use only the public API. Internal tests bind provider vectors
and canonical identity framing directly.

Shared fixtures expose semantic options rather than construction noise. Each
projected or excluded fact is varied independently so one field cannot mask
another regression.

## Documentation

Markdown uses ATX headings only: `#`, `##`, `###`, and so on. Setext headings
and horizontal rules are not used. Repository Markdown does not carry SPDX HTML
comments; licensing authority is kept in `COPYING`, `COPYRIGHT`, and source-file
headers where tooling consumes it.

Documentation distinguishes owner facts, adapter behavior, implementation
choices, and exclusions. It must not invent future orchestration, persistence,
compatibility, or execution semantics.

Manual pages use the restricted profile in `docs/manpage-markdown.md`. Markdown
is the canonical source; committed roff is generated release material. Review
both forms together and never edit generated roff directly.

## Review discipline

Every patch should have one review purpose. Tree movement, semantic code, tests,
and documentation are separated when practical. A release candidate is
accepted only after clean shared and static builds, both supported compilers,
sanitizers, generated metadata checks, installed-consumer checks, manual-page
lint, and exact patch replay.
