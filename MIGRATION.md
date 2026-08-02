# Migration from the in-tree adapter

The planner projection formerly shipped as an optional library inside the
pre-release `libpkgsource` repository. `libpkgsource-plan 1.0.0` is its first
independent release.

## Build metadata

Consumers now depend directly on:

```text
libpkgsource-plan >= 1.0.0
libpkgsource >= 3.0.0
libpkgplan >= 0.2.0
```

The installed shared library is `libpkgsource-plan.so.1` and the pkg-config
module is `libpkgsource-plan`.

## Source API

`project_candidate()` and `candidate_projection` retain their semantic role.
Callers must supply a `libpkgsource 3` snapshot. There is no source-syntax value
or recipe-format generation in the source authority API.

## Identities

The candidate-control identity domain is reset from the unpublished internal
`.../v2` spelling to the first public `libpkgsource-plan/candidate-control/v1`
domain. Candidate identities produced by the pre-release in-tree adapter have
no compatibility status and must not be retained as installed evidence.

## No compatibility layer

There is no compatibility alias for old SONAMEs, dependency metadata, source
syntax arguments, or candidate-control identities. Pre-release consumers migrate
atomically with `libpkgsource 3.0.0`.
