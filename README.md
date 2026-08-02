# libpkgsource-plan

`libpkgsource-plan` is the explicit composition boundary between sealed
`libpkgsource` authority and candidate facts owned by `libpkgplan`.

It accepts one complete `pkgsource::source_snapshot`, constructs the planner
facts that are representable by the current planner API, and returns both the
candidate and the source snapshot that issued it.

## Projection

The adapter transfers:

- normalized runtime package requirements;
- pre-remove and post-remove program material;
- normalized target architecture control;
- package name, version, distribution release, and package-release identity.

Everything else remains source authority. The adapter does not parse source
documents, resolve requirements, select packages, construct transaction
operations, execute programs, or store evidence.

## Public API

```cpp
#include <libpkgsource-plan/libpkgsource-plan.h>

#include <utility>

auto projection =
    pkgsource::plan_adapter::project_candidate(std::move(snapshot));

const auto& candidate = projection.candidate();
const auto& source = projection.source();
```

The umbrella header is the normal consumer entry point. The component header
`<libpkgsource-plan/adapter.h>` remains available for precise includes.

## Contracts

- `DESIGN.md` defines ownership and projection boundaries.
- `CANDIDATE-CONTROL-IDENTITY-1.md` defines the durable identity framing.
- `TESTING.md` maps each test to the contract it proves.
- `CODESTYLE.md` defines the repository engineering standard.
- `MIGRATION.md` records the split from the pre-release in-tree adapter.
