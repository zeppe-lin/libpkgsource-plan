# Contributing to libpkgsource-plan

## Boundary first

This repository owns one translation boundary: sealed `libpkgsource` authority
to candidate facts already representable by `libpkgplan`.

A change must not add source parsing, requirement resolution, package selection,
transaction construction, program execution, evidence storage, or fields that
neither owner library exposes.

Use the exact authoritative owner tags or bundles. Do not reconstruct an owner
API from reports, generated patches, or memory.

## Change discipline

Keep formatting, semantic code, tests, and documentation in separate commits
when practical. Compute identity material before moving its source values. Use
named protocol constants and explicit phase mappings. Keep cryptographic
provider APIs and headers inside `src/internal`.

Every projected field requires a positive behavioral test. Every excluded field
requires an independent stability test. Package release must remain separate
from candidate-control identity.

Public API changes require Doxygen updates, canonical Markdown manual updates,
regenerated roff, public-header consumer tests, the exact ELF export manifest,
and an ABI decision. Identity changes additionally require
a new normative identity document and fixed vectors. A new SHA-256 provider
requires provider vectors and the complete existing identity suite, but not a
new identity version.

## Manual pages

Edit `docs/man/*.md`, not `docs/man/generated/*`. Run
`tools/update-man-pages.sh --write`, review the generated roff in the same
commit, and run `tools/update-man-pages.sh --check` before submission. The
restricted source and Pandoc 3.x compatibility profile is defined in
`docs/manpage-markdown.md`. Do not bypass the project roff canonicalizer or
commit converter-only churn caused by an equivalent writer spelling.

## HTML documentation

Changes to canonical Markdown, public Doxygen, HTML source assets, or output
layout must pass the `html-docs` build and local-link checker. Generated HTML is
not committed. Review the versioned build artifact before changing site-facing
structure or presentation.

## Acceptance

Run the matrix in `TESTING.md`. Patches that pass only the adapter test but
skip metadata, installed consumers, owner closure, manuals, staged documentation
installation, or replay are incomplete.
