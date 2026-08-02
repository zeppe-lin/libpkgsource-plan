# Testing

The test suite qualifies both sides of the composition boundary.

## Projection behavior

The adapter test proves:

- the projection retains the exact source snapshot and identity;
- package name, version, release, and release identity cross the boundary;
- run requirements become exact runtime dependency declarations;
- pre-remove and post-remove program bytes become durable removal lifecycle;
- target architectures become one normalized target-profile fact;
- an unrestricted target architecture set becomes `*`;
- changes to runtime dependencies, removal programs, or target architectures
  change candidate-control identity;
- a package-release change changes the candidate fact and release identity but
  not an otherwise identical candidate-control identity;
- changes to metadata, source inputs, build and check requirements, selected
  build profiles, lifecycle requirements, build and check programs, build
  architectures, or installation lifecycle change source identity but not the
  projected planner candidate;
- the published candidate-control fixed vector remains byte-for-byte stable.

## Boundary contracts

`projection-boundary` statically requires every intended projection access and
rejects access to excluded source facts. It also requires the complete source
snapshot to remain part of `candidate_projection`.

`public-headers` compiles the installed header independently.

`metadata` validates one public `libpkgsource >= 3.0.0` requirement, one public
`libpkgplan >= 0.2.0` requirement, and one private direct `libcrypto`
requirement.

`release-contract` validates project version, SONAME generation, dependency
floors, the first public control-identity domain, the normative identity
specification, and absence of pre-release syntax/identity generations.

## Required release matrix

Before release, run clean shared and static builds with GCC and Clang, warnings
as errors, against the exact signed `libpkgsource 3.0.0` and `libpkgplan 0.2.0`
boundaries. Run ASan and UBSan over the adapter test. Render the scdoc manual and
lint it with mandoc. Inspect `SONAME` and `NEEDED` entries and compile an
installed external consumer through pkg-config.

Static qualification must prove the complete libcrypto and owner-library
closure using `pkg-config --static`.
