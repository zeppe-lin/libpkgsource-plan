# Candidate-control identity version 1

## Ownership

`libpkgsource-plan/candidate-control/v1` identifies the complete normalized
`pkgplan::candidate_control_projection` produced by this adapter.

`libpkgsource-plan` owns the canonical digest material. `libpkgplan` owns the
typed `candidate_control_identity` and the candidate fact that binds that
identity to one package release.

The identity does not identify a package release, source snapshot, package
image, installed record, or complete `candidate_package_fact`.

## Primitive encoding

The SHA-256 input is built from these primitives:

- `u64`: one unsigned 64-bit integer in network byte order;
- `text`: `u64(byte length)` followed by the exact string bytes;
- `sequence`: `u64(element count)` followed by each encoded element.

No terminator, padding, locale conversion, Unicode normalization, host-width
integer, or C++ object representation is added.

Strings are values already validated and normalized by the owner libraries.
The resulting 32 digest bytes are imported into
`pkgplan::candidate_control_identity`. Its planner rendering is `v1:sha256:`
followed by 64 lowercase hexadecimal digits.

## Canonical record

The input is encoded in this exact order:

1. `text("libpkgsource-plan/candidate-control/v1")`;
2. sequence of planner-normalized runtime dependencies:
   - `text(expression)`;
3. sequence of planner-normalized removal lifecycle declarations:
   - `u64(phase code)`;
   - `text(format)`;
   - `text(material)`;
4. sequence of planner-normalized target-profile facts:
   - `text(name)`;
   - `text(value)`.

Phase codes are adapter protocol values, not serialized enum representations:

| Phase | Code |
| --- | ---: |
| Pre-remove | 1 |
| Post-remove | 2 |

All three sequences are read after construction of
`pkgplan::candidate_control_projection`. Their order is therefore planner-owner
canonical order, not source declaration order.

## Exclusions

Package name, version, distribution release, and package-release identity do not
participate. They remain in the separate `pkgplan::package_release` bound by the
candidate fact.

Source identity, metadata, source inputs, build and check control, build
profiles, build architectures, installation lifecycle, lifecycle requirements,
and source origin also do not participate. They remain available through the
source snapshot retained by `candidate_projection`.

## Fixed vector

For this normalized control:

```text
runtime dependencies:
  libfoo
removal lifecycle:
  pre-remove  text/x-posix-shell  "echo pre-remove\n"
  post-remove text/x-posix-shell  "echo post-remove\n"
target profile:
  pkgsource.target-architectures = x86_64
```

the canonical identity is:

```text
v1:sha256:2064db1e0c8a2934b1998aae9cd289cf4faace757dbc0f153afd017a85781795
```

The projection identity test binds this vector to the implementation.

## Evolution

A change to field participation, primitive framing, sequence order, phase
codes, owner normalization assumptions, or the domain string changes identity
semantics and requires a new identity version.

A project version or SONAME change alone does not authorize changing this
contract.
