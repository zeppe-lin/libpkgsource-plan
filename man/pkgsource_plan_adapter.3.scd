pkgsource_plan_adapter(3)

# NAME

pkgsource_plan_adapter - project sealed source authority into planner facts

# SYNOPSIS

*#include <libpkgsource-plan/adapter.h>*

*pkgsource::plan_adapter::candidate_projection*++
*pkgsource::plan_adapter::project_candidate(* \
    *pkgsource::source_snapshot source);*

# DESCRIPTION

*libpkgsource-plan* projects one complete *source_snapshot* into one
*pkgplan::candidate_package_fact*.  The returned *candidate_projection* retains
the source snapshot beside the planner fact.

The projection contains exact run requirements, pre-remove and post-remove
program bytes, normalized target architecture constraints, package release
coordinates, and the source-issued package release identity.

Build and check requirements, action-bound lifecycle requirements, selected
build profiles, profile closure, build architecture constraints, source inputs,
build and check programs, and installation lifecycle programs do not cross the
planner boundary.

# IDENTITIES

The source package-release SHA-256 bytes are transferred into the corresponding
planner release-identity value without reinterpretation.  Candidate-control
identity is computed over planner-normalized runtime dependencies, removal
lifecycle, and target-profile facts using
*libpkgsource-plan/candidate-control/v1*.

Changing runtime dependencies, removal lifecycle, or target architecture
control changes candidate-control identity.  Changing package release changes
the bound candidate fact but not an otherwise identical control identity.
Changing an excluded source fact does not change the projected candidate.

The byte-level version-one contract and fixed vector are published in
*CANDIDATE-CONTROL-IDENTITY-1.md*.

# ERRORS

Failures throw *pkgsource::plan_adapter::projection_error*.
*projection_error_code::identity* reports digest construction failure.
*projection_error_code::planner_fact* reports rejection while constructing
planner values.

# AUTHORITY

The adapter does not parse source syntax, resolve requirements, choose package
candidates, build transaction operations, execute lifecycle programs, or store
evidence.  It translates only facts already owned by the two libraries.

# ABI

The first public ABI is *libpkgsource-plan.so.1*.  The library publicly depends
on *libpkgsource* major API 3 and *libpkgplan* 0.2 or later, and privately uses
libcrypto for candidate-control identity.

# SEE ALSO

*libpkgsource*(3), *pkgsource_snapshot*(3), *libpkgplan*(3)
