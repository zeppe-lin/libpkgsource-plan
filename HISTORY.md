# History

## 1.1.0

Testing-foundation hardening release.

- Made the public `candidate_projection` constructor verify that the supplied
  planner candidate is exactly the projection admitted by its retained source
  authority. Mismatched release, control, or candidate-control identity now
  fails with `projection_error_code::source_binding`.
- Kept distinct source snapshots admissible when their planner-visible
  projection is semantically identical; excluded source authority remains
  retained rather than entering planner candidate identity.
- Qualified against and raised the public owner floor to `libpkgplan >= 0.3.1`;
  the 1.0.0 adapter remains the old-toolchain boundary for planner 0.2.x.
- Replaced the header-only installed-consumer check with a real
  `project_candidate()` call so shared/static pkg-config qualification exercises
  the complete `libpkgsource`, `libpkgplan`, and private `libcrypto` closure.
- Reorganized tests by unit, integration, protocol, mechanism, header,
  installed-consumer, support, and repository-contract ownership.

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
- Completed Doxygen namespace coverage and self-contained include ownership.
- Added independent content, identity, exclusion, public-header, metadata,
  owner-boundary, documentation, style, and release contract tests.
- Added Doxygen, restricted Markdown/Pandoc manual generation, mandoc lint,
  formatter, compiler, sanitizer, installed-consumer, ELF, static-closure, and
  replay qualification policy.
- Made Markdown the canonical manual source and committed generated roff so
  ordinary builds do not require Pandoc.
- Canonicalized equivalent Pandoc 3.x code-block and bullet output so generated
  roff remains stable across qualified writer releases.
- Installed canonical project documentation under `share/doc/libpkgsource-plan`
  with explicit `doc` and `man` installation tags and staged-layout tests.
- Added an explicitly enabled Pandoc and Doxygen HTML artifact under
  `share/htmldocs/libpkgsource-plan/1.0.0`, with local-link, path-hygiene, and
  `DESTDIR` installation checks.
- Established `docs/code-style.md`, `.clang-format`, and `.editorconfig` as the
  house engineering baseline, with clang-format 17 pinned for reproducible layout.
- Organized project knowledge, tests, and private sources by authority and
  review purpose.
- Isolated candidate-control framing, source digest import, and SHA-256
  execution from projection orchestration.
- Established `sha256_provider` with OpenSSL as the first qualified private
  provider and added direct provider and identity protocol tests.
- Hid implementation symbols by default and bound the exact GNU/Linux C++ ABI
  through a reviewed symbol manifest and generated anonymous linker script.
- Removed the premature named symbol-version node and namespace-wide export
  wildcard before the first release.

## Import boundary

The first repository import preserves the exact adapter state from
`libpkgsource 2.1.0`. It is provenance for review, not a published API or
identity compatibility promise.
