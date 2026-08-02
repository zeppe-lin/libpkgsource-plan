# Maintaining

## Release ownership

`libpkgsource-plan` has an independent project version and SONAME. Release only
against verified `libpkgsource` and `libpkgplan` boundaries. Record exact tags or
commits in qualification notes.

## Candidate-control identity

The domain `libpkgsource-plan/candidate-control/v1` and its byte framing are
published protocol. Any change to field participation, ordering, phase codes,
string framing, or owner normalization assumptions requires an explicit
identity-version decision and new fixed vectors.

Do not place package release, source origin, build/check-only control, or
installation lifecycle into candidate-control identity unless `libpkgplan`
first establishes the corresponding owner fact.

## SHA-256 providers

The `sha256_provider` option enumerates qualified implementations. Version
1.0.0 admits only `openssl`. Add a provider only after the standard provider
vectors, the candidate-control fixed vector, projection tests, sanitizers,
metadata checks, and shared/static closure pass with that provider selected.

A provider replacement must preserve exact SHA-256 bytes. An algorithm or
framing change requires a new identity protocol version. Do not add silent
provider fallback.

## Release checklist

1. Review the public header, manual, design, identity specification, migration
   note, history, and testing map together.
2. Run formatter and Doxygen checks.
3. Run GCC and Clang shared/static builds with warnings as errors for every
   admitted SHA-256 provider.
4. Run sanitizers, generated metadata, installed consumers, and static closure.
5. Inspect SONAME, ABI version node, exported symbols, and direct
   shared-library dependencies.
6. Regenerate manuals with Pandoc, verify a clean generated diff, and lint the
   committed roff with mandoc.
7. Run repository contracts, `git diff --check`, and `git fsck`.
8. Replay the patch series independently and compare final trees.

Do not tag a tree with skipped or inferred qualification results.
