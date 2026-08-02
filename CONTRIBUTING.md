# Contributing

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
named protocol constants and explicit phase mappings.

Every projected field requires a positive behavioral test. Every excluded field
requires an independent stability test. Package release must remain separate
from candidate-control identity.

Public API changes require Doxygen updates, manual-page updates, public-header
consumer tests, and an ABI decision. Identity changes additionally require a
new normative identity document and fixed vectors.

## Acceptance

Run the matrix in `TESTING.md`. Patches that pass only the adapter test but skip
metadata, installed consumers, owner closure, manuals, documentation, or replay
are incomplete.
