% PKGSOURCE_PLAN_ADAPTER(3) libpkgsource-plan 1.0.0 | libpkgsource-plan

# NAME

pkgsource_plan_adapter - project sealed source authority into planner facts

# SYNOPSIS

```cpp
#include <libpkgsource-plan/libpkgsource-plan.h>

pkgsource::plan_adapter::candidate_projection
pkgsource::plan_adapter::project_candidate(
    pkgsource::source_snapshot source);
```

# DESCRIPTION

`project_candidate()` consumes one complete sealed
`pkgsource::source_snapshot` and returns a
`pkgsource::plan_adapter::candidate_projection` containing both the original
source authority and one `pkgplan::candidate_package_fact` derived from it.

The planner fact contains:

- each normalized runtime package requirement as one planner runtime
  dependency;
- pre-remove and post-remove programs as exact removal-lifecycle material with
  format `text/x-posix-shell`;
- the normalized target architecture set as the target-profile fact
  `pkgsource.target-architectures`;
- package name, version, distribution release, and the source-issued package
  release identity.

An empty target architecture set is represented by `*`. A constrained set is a
comma-separated sequence in the normalized order supplied by `libpkgsource`.

Build and check requirements, lifecycle requirements, selected profiles, build
architecture constraints, source inputs, build and check programs, pre-install
and post-install programs, source origin, and source-snapshot identity do not
become planner candidate control. They remain available through the retained
source snapshot.

# OWNERSHIP

The input snapshot is taken by value. The returned
`pkgsource::plan_adapter::candidate_projection` owns that snapshot and the
projected planner fact. Passing an rvalue transfers the snapshot without an
additional copy.

The adapter does not reopen source paths or mutate the snapshot.

# IDENTITIES

The source package-release SHA-256 bytes are imported into the corresponding
planner release-identity value without recomputation.

Candidate-control identity is computed over the constructed,
planner-normalized runtime dependencies, removal lifecycle, and target-profile
facts using the domain `libpkgsource-plan/candidate-control/v1`. Package release
coordinates and source-snapshot identity do not participate in
candidate-control identity.

The byte-level contract and fixed vector are specified in
`docs/protocols/candidate-control-identity-v1.md` in the source distribution.

# ERRORS

`pkgsource::plan_adapter::projection_error` is thrown for adapter-owned
projection failures.

`pkgsource::plan_adapter::projection_error_code::identity`
: Identity framing, SHA-256 processing, or source release-identity decoding
  failed.

`pkgsource::plan_adapter::projection_error_code::planner_fact`
: A projected value raised `pkgplan::fact_error` while constructing a
  `libpkgplan` fact. Other standard exceptions are not reclassified.

# AUTHORITY

The adapter translates only facts already owned by `libpkgsource` and
`libpkgplan`. It does not parse source documents, resolve requirements, choose
package candidates, construct transaction operations, execute programs, or
store evidence.

# ABI

The public library is `libpkgsource-plan.so.1`. The public API depends on
`libpkgsource` 3.0 or later and `libpkgplan` 0.2 or later. Libcrypto is a private
implementation dependency used for candidate-control identity.

# SEE ALSO

`libpkgsource(3)`, `pkgsource_snapshot(3)`, `libpkgplan(3)`
