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

- `docs/architecture.md` defines ownership, tree, projection, and provider
  boundaries.
- `docs/protocols/candidate-control-identity-v1.md` defines the durable identity
  framing.
- `docs/testing.md` maps public, internal, and repository tests to their
  contracts.
- `docs/abi.md` defines the reviewed ELF export boundary.
- `docs/code-style.md` defines the repository engineering standard.
- `docs/manpage-markdown.md` defines the restricted Markdown-to-man profile.
- `docs/history/in-tree-adapter-migration.md` records the split from the
  pre-release in-tree adapter.


## Installed documentation

Canonical project documentation is installed under
`share/doc/libpkgsource-plan`. The generated manual remains in the normal
`share/man/man3` hierarchy. Both layouts honor the configured prefix and
`DESTDIR` staging.
