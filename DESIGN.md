# Design

## Purpose

`libpkgsource` and `libpkgplan` own different semantic domains. A source
snapshot describes the complete declared package source. A planner candidate
contains only package-control facts required to evaluate installed-state
transitions.

`libpkgsource-plan` is the explicit translation boundary between them. It is a
separate repository because it depends on both owners and must evolve only when
either public contract changes. Neither authority library depends back on the
adapter.

## Public interface

The complete public projection API is exposed through
`<libpkgsource-plan/libpkgsource-plan.h>`. The component header
`<libpkgsource-plan/adapter.h>` remains installed for precise includes, while
consumer documentation and installed-consumer qualification use the umbrella
header as the stable default entry point.

## Projected facts

`project_candidate()` translates:

1. Each sealed run requirement into one exact planner runtime dependency.
2. Each pre-remove and post-remove program into durable planner removal
   lifecycle control, preserving exact program bytes and declaring the format
   `text/x-posix-shell`.
3. The normalized target architecture set into the planner target-profile fact
   `pkgsource.target-architectures`. An empty set is represented as `*`; a
   constrained set is a comma-separated sequence in core-normalized order.
4. The source package release name, version, release, and source-issued package
   release identity into the corresponding planner release value.

The adapter computes candidate-control identity over the planner-normalized
control projection using the domain
`libpkgsource-plan/candidate-control/v1`. `libpkgplan` owns the typed identity
and the candidate fact that binds it to one package release; this adapter owns
the canonical control material. The exact framing is specified in
`CANDIDATE-CONTROL-IDENTITY-1.md`.

Package release coordinates and release identity cross the boundary, but they
do not participate in candidate-control identity. They are the separate release
fact bound by `pkgplan::candidate_package_fact`.

## Excluded facts

The projection deliberately excludes:

- build and check requirements;
- action-bound lifecycle requirements;
- selected build profiles and retained profile closure;
- build architecture constraints;
- source input declarations;
- build and check program bytes;
- pre-install and post-install program bytes;
- source-snapshot identity as planner control.

Those facts may affect source authority, resolution, build, checking, lifecycle
execution, or evidence storage, but they are not candidate control representable
or owned by the current `libpkgplan` API.

Changing an excluded semantic fact may change the retained source snapshot
identity but must not change the projected planner candidate. Changing runtime
dependencies, removal lifecycle, or target-profile facts must change
candidate-control identity. Changing the package release must change the
candidate fact while leaving an otherwise identical control identity unchanged.

## Provenance retention

`candidate_projection` stores the complete source snapshot by value beside the
planner candidate. This prevents projection from erasing the authority that
issued the fact and lets orchestration verify the exact source identity without
asking the planner to absorb source semantics.

The source snapshot is not serialized, resolved, reopened, or modified by this
adapter.

## Failure model

`projection_error` distinguishes failures while constructing SHA-256 identity
material from failures reported while constructing planner facts. A valid
source snapshot should normally project without semantic failure; the typed
error surface exists for cryptographic backend failure and for future planner
validation changes.

## Dependencies

The library has public dependencies on `libpkgsource` and `libpkgplan`. It has a
private direct dependency on libcrypto because this adapter computes the
candidate-control digest itself. The crypto dependency is not inherited merely
from either owner.
