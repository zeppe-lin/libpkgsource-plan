# Testing

## Executable behavior

`projection-content` proves the exact owner facts that cross the boundary:
package release, release identity, runtime dependencies, removal lifecycle, and
target architecture control. It also proves wildcard handling and source-owner
architecture ordering.

`projection-identity` binds the published fixed vector. It varies each projected
control group independently and proves that package release remains separate
from candidate-control identity.

`projection-exclusions` varies each excluded semantic fact independently. Every
case must change source identity while leaving the planner candidate equal. A
separate case proves that source origin is retained provenance but does not
change source identity or planner control.

`public-umbrella` and `public-component` compile the two installed entry points
in separate translation units and bind the public function type.

## Repository contracts

`owner-boundary` checks only surfaces that cannot be proved through behavior:
public owner headers, direct dependency declarations, absence of YAML, codec,
and image authority, retention of the complete source snapshot, and narrow
translation of planner-owned validation failures.

`metadata` parses generated pkg-config output. It requires exactly one public
`libpkgsource` dependency, exactly one public `libpkgplan` dependency, and
exactly one private `libcrypto` dependency.

`documentation-contract` checks required documents, structural sections, public
include examples, and owner-boundary terminology without binding incidental
prose.

`style-contract` checks the repository style authority and Markdown rules.
`manpage-source` checks the restricted manual-page Markdown profile, title,
section order, and canonical synopsis. When Pandoc is available,
`manpage-generated` regenerates the roff and rejects any difference from the
committed release artifact.

When `clang-format-17` is available, `format` checks every C++ source, header,
and test against `.clang-format` and rejects a different formatter major.

`release-contract` checks project version, SONAME generation, dependency floors,
installed headers, pkg-config promotion, identity domain, fixed vector, and
absence of unpublished identity or syntax generations.

## Release qualification

Before tagging, run clean shared and static builds with GCC and Clang against
the exact supported owner boundaries. Promote warnings to errors. Run ASan and
UBSan. Generate Doxygen without warnings. Regenerate manual pages with Pandoc,
require a clean generated diff, and lint the committed roff with mandoc. The
ordinary build must install the committed roff without requiring Pandoc.

Install each build into an empty prefix. Compile and run an external consumer
through `pkg-config`; use `pkg-config --static` for the static closure. Inspect
the shared object for the expected SONAME and direct `NEEDED` entries.

Finally, run `git diff --check`, `git fsck`, and an independent `git am` replay.
The replay tree, not an assumed commit identifier, is the reproducibility
invariant.
