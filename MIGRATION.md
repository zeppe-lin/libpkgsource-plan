# Migration from the in-tree adapter

The planner projection previously existed as an optional component in the
pre-release `libpkgsource` repository. `libpkgsource-plan 1.0.0` is its first
independent release.

## Build dependency

Consumers now require:

```text
libpkgsource-plan >= 1.0.0
libpkgsource >= 3.0.0
libpkgplan >= 0.2.0
```

The pkg-config module is `libpkgsource-plan`. The shared library is
`libpkgsource-plan.so.1`.

## Include path

Use the umbrella header:

```cpp
#include <libpkgsource-plan/libpkgsource-plan.h>
```

The component header `<libpkgsource-plan/adapter.h>` is also installed.

## Source API

`project_candidate()` still returns a planner candidate retained beside its
issuing source snapshot. Callers supply a parser-neutral `libpkgsource 3`
snapshot. No source-syntax value or recipe-format generation crosses the API.

## Identity reset

The unpublished in-tree candidate-control domain has no compatibility status.
The first public contract is
`libpkgsource-plan/candidate-control/v1`, specified in
`CANDIDATE-CONTROL-IDENTITY-1.md`.

Existing pre-release candidate identities must not be imported as installed or
persistent evidence.

## No compatibility layer

There is no compatibility alias for old SONAMEs, dependency metadata, syntax
arguments, or unpublished candidate-control identities. Pre-release consumers
migrate atomically with `libpkgsource 3.0.0`.
