# History

## 1.0.0

First independent source-to-planner projection release.

- Extracted the adapter from the pre-release `libpkgsource 2.1.0` tree through a
  reviewable provenance commit.
- Rebased the projection on parser-neutral `libpkgsource 3.0.0` authority and
  `libpkgplan 0.2.0` candidate facts.
- Published `<libpkgsource-plan/libpkgsource-plan.h>` as the umbrella header and
  retained `<libpkgsource-plan/adapter.h>` as the component header.
- Projected normalized runtime package requirements, removal lifecycle, target
  architectures, package release coordinates, and package-release identity.
- Retained the complete issuing source snapshot beside the planner candidate.
- Published the first candidate-control identity contract as
  `libpkgsource-plan/candidate-control/v1` with a normative byte specification
  and fixed vector.
- Fixed identity construction so control is hashed before ownership is moved
  into the planner fact.
- Mapped identity phase codes explicitly instead of serializing enum
  representation.
- Limited planner-failure translation to `pkgplan::fact_error`; unrelated
  standard exceptions retain their original type.
- Published `libpkgsource` and `libpkgplan` exactly once as public pkg-config
  requirements and retained libcrypto as one private requirement.
- Added independent content, identity, exclusion, public-header, metadata,
  owner-boundary, documentation, style, and release contract tests.
- Added Doxygen, scdoc, formatter, compiler, sanitizer, installed-consumer, ELF,
  static-closure, and replay qualification policy.
- Established `CODESTYLE.md`, `.clang-format`, and `.editorconfig` as the house
  engineering baseline, with clang-format 17 pinned for reproducible layout.

## Import boundary

The first repository import preserves the exact adapter state from
`libpkgsource 2.1.0`. It is provenance for review, not a published API or
identity compatibility promise.
