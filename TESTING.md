# Testing libpkgsource-plan

## Unit projection behavior

`projection-content` proves the exact owner facts that cross the boundary:
package release, release identity, runtime dependencies, removal lifecycle, and
target architecture control. It also proves wildcard handling and source-owner
architecture ordering.

`projection-exclusions` varies each excluded semantic fact independently. Every
case must change source identity while leaving the planner candidate equal. A
separate case proves that source origin is retained provenance but not semantic
identity.

## Integration seams

`projection-identity` binds the published fixed vector through the real
`libpkgsource` → adapter → `libpkgplan` path. It varies each projected control
group independently and proves that package release remains separate from
candidate-control identity.

`projection-binding` directly exercises the public `candidate_projection`
constructor. It rejects a candidate from different projected control, rejects a
candidate from another package release, and accepts a distinct source snapshot
when its planner-visible projection is exactly equivalent.

## Protocol and mechanism behavior

The `mechanism` `sha256-provider` test binds the selected provider to standard
empty-input, incremental `abc`, and exact binary-input vectors.

The `protocol` `candidate-control-identity` test calls the private protocol
encoder directly. It binds the published version-one vector and proves that
encoding observes planner-normalized order rather than caller insertion order.

The `protocol` `source-release-identity` test proves that validated source
SHA-256 bytes enter the planner package-release digest domain unchanged.

These tests do not make private headers public. Meson links them against the
non-installed private identity archive used by the shared library.

## Public headers

`public-umbrella` and `public-component` compile and execute consumers through
the aggregate and component include paths. CI then compiles and runs
`tests/installed/consumer.cpp` solely through generated installed pkg-config
metadata; that consumer calls `project_candidate()` so static qualification
pulls the complete private closure rather than merely parsing headers.

## Repository contracts

`owner-boundary` proves that parser, codec, image, and provider details do not
cross the public adapter boundary. It also proves that OpenSSL appears only in
the selected provider implementation.

`metadata` requires `libpkgsource` and `libpkgplan` publicly exactly once and
`libcrypto` privately exactly once for the current provider.

`abi-surface` compares the complete shared-library dynamic symbol table with
`abi/libpkgsource-plan.exports`. It also rejects internal, standard-library, and
premature named symbol-version exports.

`release-contract`, `documentation-contract`, `documentation-install-contract`,
`html-docs-contract`, `style-contract`,
`manpage-source`, `manpage-normalizer`, `manpage-generated`, and `format` bind
release metadata, knowledge layout, explicit documentation installation,
Markdown policy, Pandoc 3.x writer normalization, generated roff, and mechanical
C++ format. `installed-docs` validates the staged canonical documentation
tree. `html-docs` validates the rendered tree and local links;
`installed-html-docs` validates its versioned `DESTDIR` installation.

## Release qualification

The release matrix covers:

- GCC and Clang;
- separate shared and static builds;
- warnings as errors;
- GCC and Clang ASan/UBSan runs;
- the complete owner-library closure;
- all unit, integration, protocol, mechanism, and public-header tests;
- installed shared and static consumers through pkg-config;
- SONAME and direct shared-library dependencies;
- Doxygen API HTML, project Markdown HTML, Pandoc man-page regeneration, and
  mandoc lint;
- repository contracts, `git diff --check`, `git fsck`, and independent
  `git am` replay.

A provider is qualified only when the same matrix passes with that provider
selected explicitly.
