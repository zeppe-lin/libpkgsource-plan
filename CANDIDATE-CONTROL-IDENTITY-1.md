# Candidate control identity version 1

## Scope

`libpkgsource-plan/candidate-control/v1` identifies the complete normalized
`pkgplan::candidate_control_projection` produced by this adapter. It does not
identify a package release, a source snapshot, an artifact, an installed
record, or a complete `pkgplan::candidate_package_fact`.

`libpkgplan` owns the typed `candidate_control_identity` value and the candidate
fact that binds that identity to one package release. `libpkgsource-plan` owns
the canonical material from which the control identity is computed.

## Primitive encoding

The digest input is a byte sequence built from these primitives:

- `u64`: one unsigned 64-bit integer in network byte order;
- `text`: `u64(byte length)` followed by the exact string bytes;
- `sequence`: `u64(element count)` followed by each encoded element.

String bytes are those already accepted and normalized by the two owner
libraries. No terminator, padding, locale conversion, Unicode normalization, or
host-width integer representation is added.

The SHA-256 digest is imported as the exact bytes of a
`pkgplan::candidate_control_identity`. Its canonical planner rendering is
`v1:sha256:` followed by 64 lowercase hexadecimal digits.

## Record

The input sequence is encoded in this exact order:

1. `text("libpkgsource-plan/candidate-control/v1")`;
2. sequence of normalized runtime dependencies:
   - `text(expression)`;
3. sequence of normalized removal lifecycle declarations:
   - `u64(phase)` where `pre-remove` is 1 and `post-remove` is 2;
   - `text(format)`;
   - `text(material)`;
4. sequence of normalized target-profile facts:
   - `text(name)`;
   - `text(value)`.

The three sequences are read from a constructed
`pkgplan::candidate_control_projection`, after planner normalization. Their
stored order is therefore canonical owner order, not source declaration order.

## Exclusions

Package name, version, release, and package-release identity do not participate
in candidate-control identity. They remain in the separate
`pkgplan::package_release` carried by `candidate_package_fact`.

Source identity, metadata, source inputs, build and check control, build
profiles, build architectures, installation lifecycle, and lifecycle
requirements also do not participate. They remain available through the source
snapshot retained by `candidate_projection`.

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

The executable adapter test binds this vector to the implementation.

## Evolution

Changing field participation, primitive framing, sequence order, phase codes,
normalization assumptions, or the domain string changes identity semantics and
requires a new identity version. A new project or SONAME version alone does not
permit silently changing this contract.
