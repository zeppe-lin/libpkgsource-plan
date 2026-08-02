# Testing

## Projection behavior

`projection-content` proves the exact owner facts that cross the boundary:
package release, release identity, runtime dependencies, removal lifecycle, and
target architecture control. It also proves wildcard handling and source-owner
architecture ordering.

`projection-identity` binds the published fixed vector through the public
adapter. It varies each projected control group independently and proves that
package release remains separate from candidate-control identity.

`projection-exclusions` varies each excluded semantic fact independently. Every
case must change source identity while leaving the planner candidate equal. A
separate case proves that source origin is retained provenance but not semantic
identity.

## Internal identity behavior

`sha256-provider` binds the selected provider to standard empty-input,
incremental `abc`, and exact binary-input vectors.

`candidate-control-identity` calls the private protocol encoder directly. It
binds the published version-one vector and proves that encoding observes
planner-normalized order rather than caller insertion order.

`source-release-identity` proves that validated source SHA-256 bytes enter the
planner package-release digest domain unchanged.

These tests do not make private headers public. Meson links them against the
non-installed private identity archive used by the shared library.

## Public headers

`public-umbrella` and `public-component` compile and execute consumers through
the aggregate and component include paths. CI repeats the umbrella consumer
after installation through generated pkg-config metadata.

## Repository contracts

`owner-boundary` proves that parser, codec, image, and provider details do not
cross the public adapter boundary. It also proves that OpenSSL appears only in
the selected provider implementation.

`metadata` requires `libpkgsource` and `libpkgplan` publicly exactly once and
`libcrypto` privately exactly once for the current provider.

`abi-surface` verifies the shared-library version node, required public
symbols, and absence of internal or standard-library exports.

`release-contract`, `documentation-contract`, `style-contract`,
`manpage-source`, `manpage-generated`, and `format` bind release metadata,
knowledge layout, Markdown policy, generated roff, and mechanical C++ format.

## Release qualification

The release matrix covers:

- GCC and Clang;
- separate shared and static builds;
- warnings as errors;
- GCC and Clang ASan/UBSan runs;
- the complete owner-library closure;
- all projection, internal identity, and public-header tests;
- installed shared and static consumers through pkg-config;
- SONAME and direct shared-library dependencies;
- Doxygen, Pandoc regeneration, and mandoc lint;
- repository contracts, `git diff --check`, `git fsck`, and independent
  `git am` replay.

A provider is qualified only when the same matrix passes with that provider
selected explicitly.
