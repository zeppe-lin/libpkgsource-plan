# libpkgsource-plan

`libpkgsource-plan` is the narrow composition adapter between sealed
`libpkgsource` authority and `libpkgplan` candidate facts.

It accepts one complete `pkgsource::source_snapshot`, projects only the facts
that the planner owns, and retains the original snapshot beside the projected
candidate:

- exact run requirements;
- pre-remove and post-remove program bytes;
- target architecture constraints;
- package release coordinates and source-issued release identity.

Build and check requirements, build profiles, source inputs, build and check
programs, build architecture constraints, installation lifecycle programs, and
all other source authority remain outside planner candidate control.

```cpp
#include <libpkgsource-plan/adapter.h>

auto projection = pkgsource::plan_adapter::project_candidate(snapshot);
const auto& candidate = projection.candidate();
const auto& source = projection.source();
```

The adapter does not parse documents, resolve requirements, select packages,
construct transaction operations, or store evidence. Candidate-control identity
is specified byte-for-byte in `CANDIDATE-CONTROL-IDENTITY-1.md`; see `DESIGN.md`
and `TESTING.md` for the full projection contract.
