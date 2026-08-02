# History

## 1.0.0 — unreleased

First independent source-to-planner projection release.

- Extracted the planner adapter, public header, manual, and tests from the
  pre-release `libpkgsource 2.1.0` repository through a reviewable import
  commit.
- Rebased the adapter on the parser-neutral `libpkgsource 3.0.0` source model
  and removed the obsolete source-syntax generation argument.
- Reset the unpublished candidate-control identity domain to the first public
  `libpkgsource-plan/candidate-control/v1` contract.
- Preserved the narrow projection of run requirements, removal lifecycle,
  target architectures, package release facts, and release identity.
- Added tests proving that included facts change candidate identity, excluded
  source facts do not, and complete source provenance remains retained.
- Added projection-boundary, release-metadata, public-header, shared/static,
  sanitizer, and manual qualification contracts.

## Import boundary

The first repository commit imports the component exactly from
`libpkgsource 2.1.0`. It is review provenance, not a published API or identity
contract.
