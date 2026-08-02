# Design

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

## Public interface

The complete API is exposed through
`<libpkgsource-plan/libpkgsource-plan.h>`. The component header
`<libpkgsource-plan/adapter.h>` is also installed.

`project_candidate()` takes a source snapshot by value and returns a
`candidate_projection`. The result owns both the source snapshot and the
projected planner fact. This preserves the authority that issued the narrower
fact.

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
byte framing is specified in `CANDIDATE-CONTROL-IDENTITY-1.md`.

Protocol phase codes are mapped explicitly. The digest does not depend on the
compiler representation or underlying numeric values of a C++ enum.

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

`projection_error_code::identity` reports failures while decoding the source
package-release digest or constructing candidate-control SHA-256 material.

`projection_error_code::planner_fact` reports a `pkgplan::fact_error` raised
while constructing planner-owned values. The original diagnostic text is
retained in the adapter error message. Unrelated standard exceptions are not
reclassified as planner validation failures.

The adapter performs no retry, fallback, or policy substitution.

## Dependencies and ABI

The public API depends on `libpkgsource >= 3.0.0` and `libpkgplan >= 0.2.0`.
Libcrypto is a private implementation dependency used only for
candidate-control SHA-256 construction.

The first public shared-library ABI is `libpkgsource-plan.so.1`.

## Documentation pipeline

Manual-page semantics are maintained in restricted Markdown. Pandoc is a
maintainer regeneration tool, not an ordinary build dependency. The repository
commits generated `man(7)` roff, Meson installs that artifact, and CI proves that
regeneration is clean before mandoc lints the installed representation.

This keeps one reviewable prose source while making package builds independent
of the document compiler.
