# Architecture

## Authority boundary

`libpkgsource` owns complete sealed package-source authority. `libpkgplan` owns
facts admitted to package planning. Neither owner library depends on this
adapter.

```text
pkgsource::source_snapshot
            |
            | libpkgsource-plan
            v
pkgplan::candidate_package_fact
```

The adapter is a separate repository because it depends on both owners and must
change only when one of their public contracts changes.

## Repository layout

The tree separates public ABI, projection orchestration, identity protocol,
cryptographic provider, tests, and project knowledge.

```text
include/libpkgsource-plan/   installed public API
src/adapter.cpp              source-to-plan orchestration
src/internal/                private identity and digest implementation
man/                         canonical Markdown and generated roff
docs/protocols/              normative identity protocols
docs/history/                retained migration provenance
tests/projection/            public behavioral contracts
tests/internal/              private identity and provider contracts
tests/public/                installed-header contracts
tests/contracts/             repository and generated-artifact contracts
tests/support/               shared semantic fixtures
tools/                       maintainer-only generators
```

Files under `src/internal` and `tests/internal` are not installed and are not
public API. Root-level README, HISTORY, CONTRIBUTING, MAINTAINING, COPYING, and
COPYRIGHT remain conventional project entry points.

## Public interface

The complete API is exposed through
`<libpkgsource-plan/libpkgsource-plan.h>`. The component header
`<libpkgsource-plan/adapter.h>` is also installed.

`project_candidate()` takes a source snapshot by value and returns a
`candidate_projection`. The result owns both the source snapshot and the
projected planner fact. This preserves the authority that issued the narrower
fact.

Public Doxygen belongs in installed headers. Implementation files document
internal invariants, ordering, ownership transfer, and error translation
without duplicating the public contract.

## Projection pipeline

`adapter.cpp` follows one explicit order:

1. read the sealed recipe from the retained source snapshot;
2. construct planner-owned runtime dependency, removal lifecycle, and target
   profile facts;
3. let `pkgplan::candidate_control_projection` normalize those facts;
4. import the source-owned package-release identity into the planner digest
   domain;
5. compute candidate-control identity over normalized planner control;
6. move release and control into `candidate_package_fact`;
7. return the candidate beside the complete issuing source snapshot.

Candidate-control identity is computed before control is moved. Function
argument evaluation order must never determine identity material.

## Projection map

| Source authority | Planner fact | Transformation |
| --- | --- | --- |
| `sealed_recipe::run_requirements()` | `runtime_dependency_declaration` | One normalized package name becomes one planner expression. |
| Pre-remove and post-remove programs | `removal_lifecycle_declaration` | Phase is mapped explicitly; format is `text/x-posix-shell`; material bytes are preserved. |
| Normalized target architectures | `target_profile_fact` | The fact name is `pkgsource.target-architectures`; an empty set becomes `*`; otherwise names are comma-separated in source-normalized order. |
| `package_release` coordinates | `pkgplan::package_release` | Name and version are copied; the numeric distribution release is rendered in decimal. |
| Source package-release identity | Planner package-release identity | The 32 SHA-256 bytes are imported without recomputation. |

The planner constructs and normalizes the complete
`candidate_control_projection` before the adapter computes its control
identity.

## Candidate-control identity

`candidate_package_fact::identity()` identifies normalized candidate control,
not the complete candidate fact. Package release coordinates and release
identity are bound beside that control and do not participate in the control
digest.

The adapter owns the canonical digest material. `libpkgplan` owns the typed
`candidate_control_identity` and the candidate fact that carries it. The exact
byte framing is specified in
`docs/protocols/candidate-control-identity-v1.md`.

Protocol phase codes are mapped explicitly. The digest does not depend on the
compiler representation or underlying numeric values of a C++ enum.

## SHA-256 provider boundary

`src/internal/sha256.h` is the provider-neutral internal contract.
`src/internal/sha256_openssl.cpp` is the current qualified implementation and
the only source file permitted to include OpenSSL headers.

Meson selects the provider through `sha256_provider`. Version 1.0.0 admits only
`openssl`; the single choice records the qualified implementation set rather
than promising untested alternatives. A new provider must pass the same empty,
incremental, binary, and candidate-control fixed vectors before it becomes an
admitted choice.

Changing from one correct SHA-256 provider to another is an implementation
change. Changing the hash algorithm, digest width, canonical framing, or domain
string changes identity semantics and requires a new candidate-control identity
version.

No provider type, header, error, or configuration value appears in the public
API.

## Excluded source authority

The current planner candidate API has no owner field for:

- package metadata;
- source input declarations;
- build and check requirements;
- lifecycle requirements;
- selected build profiles and retained profile closure;
- build architecture constraints;
- build and check programs;
- pre-install and post-install programs;
- source origin or source-snapshot identity.

Those facts therefore do not cross the planner boundary. Semantic changes to
an excluded recipe fact can change source identity while leaving the projected
candidate unchanged. Diagnostic source origin is retained by the snapshot but
is not part of source identity or planner control.

## Failure model

`projection_error_code::identity` reports failure while importing source
package-release digest bytes or executing the selected SHA-256 provider for
candidate-control identity.

`projection_error_code::planner_fact` reports a `pkgplan::fact_error` raised
while constructing planner-owned values. The original diagnostic text is
retained in the adapter error message. Allocation failures, logic failures, and
unrelated standard exceptions are not reclassified.

The adapter performs no retry, provider fallback, or policy substitution.

## Dependencies and ABI

The public API depends on `libpkgsource >= 3.0.0` and `libpkgplan >= 0.2.0`.
The selected SHA-256 provider is a private implementation dependency. With the
current provider, generated pkg-config metadata records `libcrypto` privately.

The first public shared-library ABI is `libpkgsource-plan.so.1`. Symbols are
hidden by default and the linker map publishes only the
`pkgsource::plan_adapter` ABI under `LIBPKGSOURCE_PLAN_1.0`. Private source
layout, helper symbols, standard-library instantiations, and provider
implementation are not ABI.

## Documentation pipeline

Manual-page semantics are maintained in restricted Markdown. Pandoc is a
maintainer regeneration tool, not an ordinary build dependency. The repository
commits generated `man(7)` roff, Meson installs that artifact, and CI proves that
regeneration is clean before mandoc lints the installed representation.
