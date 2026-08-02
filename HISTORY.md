# History

## 1.0.0

First independent source-to-planner projection release.

- Published `libpkgsource` and `libpkgplan` as public pkg-config requirements
  exactly once while retaining `libcrypto` as a private implementation
  dependency.
- Extracted the planner adapter, public header, manual, and tests from the
  pre-release `libpkgsource 2.1.0` repository through a reviewable import
  commit.
- Rebased the adapter on the parser-neutral `libpkgsource 3.0.0` source model
  and removed the obsolete source-syntax generation argument.
- Reset the unpublished candidate-control identity domain to the first public
  `libpkgsource-plan/candidate-control/v1` contract.
- Preserved the narrow projection of run requirements, removal lifecycle,
  target architectures, package release facts, and release identity.
- Fixed control identity construction so normalized control is hashed before
  ownership is transferred into the planner fact.
- Added a normative candidate-control identity specification and fixed vector.
- Added tests proving that projected control changes control identity, package
  release changes only the bound release fact, excluded source facts do not
  cross the boundary, and complete source provenance remains retained.
- Added projection-boundary, release-metadata, public-header, and
  documentation contract tests.
- Published the required shared/static, compiler, sanitizer, installed-consumer,
  ELF, and manual qualification matrix without claiming it before execution.

## Import boundary

The first repository commit imports the component exactly from
`libpkgsource 2.1.0`. It is review provenance, not a published API or identity
contract.
