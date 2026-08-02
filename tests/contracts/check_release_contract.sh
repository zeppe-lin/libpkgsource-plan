#!/bin/sh
# SPDX-FileCopyrightText: 2026 Alexandr Savca
# SPDX-License-Identifier: GPL-3.0-or-later
set -eu
root=$1
fail()
{
  echo "release-contract-test: $*" >&2
  exit 1
}

grep -F "version: '1.0.0'" "$root/meson.build" >/dev/null ||
  fail 'project version is not 1.0.0'
grep -F '## 1.0.0' "$root/HISTORY.md" >/dev/null ||
  fail 'release history is not finalized'
grep -F "soversion: '1'" "$root/src/meson.build" >/dev/null ||
  fail 'library SONAME generation is not 1'
grep -F "'../include/libpkgsource-plan/libpkgsource-plan.h'" \
  "$root/src/meson.build" >/dev/null ||
  fail 'umbrella header is not installed'
grep -F "'../include/libpkgsource-plan/export.h'" \
  "$root/src/meson.build" >/dev/null ||
  fail 'public visibility header is not installed'
grep -F "gnu_symbol_visibility: 'hidden'" \
  "$root/src/meson.build" >/dev/null ||
  fail 'shared-library implementation is not hidden by default'
[ -s "$root/abi/libpkgsource-plan.exports" ] ||
  fail 'reviewed ELF ABI manifest is missing'
[ -x "$root/tools/generate-elf-export-script.sh" ] ||
  fail 'ELF export-script generator is missing or not executable'
grep -F "input: '../abi/libpkgsource-plan.exports'" \
  "$root/src/meson.build" >/dev/null ||
  fail 'Meson does not generate export control from the ABI manifest'
[ ! -e "$root/src/libpkgsource-plan.map" ] ||
  fail 'retired wildcard ABI map remains in the source tree'
grep -F '#include <libpkgsource-plan/adapter.h>' \
  "$root/include/libpkgsource-plan/libpkgsource-plan.h" >/dev/null ||
  fail 'umbrella header does not expose the complete adapter API'
grep -F "version: '>=3.0.0'" "$root/meson.build" >/dev/null ||
  fail 'libpkgsource dependency floor is not 3.0.0'
grep -F "version: '>=0.2.0'" "$root/meson.build" >/dev/null ||
  fail 'libpkgplan dependency floor is not 0.2.0'
grep -F "'sha256_provider'" "$root/meson.options" >/dev/null ||
  fail 'SHA-256 provider option is missing'
grep -F "choices: ['openssl']" "$root/meson.options" >/dev/null ||
  fail 'qualified SHA-256 provider set is not explicit'
grep -F 'requires: [libpkgsource_dep, libpkgplan_dep]' \
  "$root/src/meson.build" >/dev/null ||
  fail 'public pkg-config dependencies are not promoted by dependency object'
if grep -E '^[[:space:]]*requires_private:' \
    "$root/src/meson.build" >/dev/null; then
  fail 'private pkg-config closure is duplicated manually'
fi
if grep -E "^[[:space:]]*requires:.*'(libpkgsource|libpkgplan|libcrypto)" \
    "$root/src/meson.build" >/dev/null; then
  fail 'pkg-config owner dependencies use string requirements'
fi
[ -s "$root/docs/man/pkgsource_plan_adapter.3.md" ] ||
  fail 'canonical Markdown manual source is missing'
[ -s "$root/docs/man/generated/pkgsource_plan_adapter.3" ] ||
  fail 'generated roff manual is missing'
[ -x "$root/tools/update-man-pages.sh" ] ||
  fail 'manual-page generator is missing or not executable'
[ -s "$root/tools/canonicalize-man-roff.awk" ] ||
  fail 'manual-page roff canonicalizer is missing or empty'
grep -F "input: 'generated/pkgsource_plan_adapter.3'" \
  "$root/docs/man/meson.build" >/dev/null ||
  fail 'ordinary builds do not install committed generated roff'
grep -F -- '--from=markdown-smart' \
  "$root/tools/update-man-pages.sh" >/dev/null ||
  fail 'manual-page generator does not bind the restricted Markdown reader'
grep -F -- '--fail-if-warnings' "$root/tools/update-man-pages.sh" >/dev/null ||
  fail 'manual-page generator does not reject Pandoc warnings'
grep -F -- '--eol=lf' "$root/tools/update-man-pages.sh" >/dev/null ||
  fail 'manual-page generator does not bind LF output'
grep -F -- '--wrap=none' "$root/tools/update-man-pages.sh" >/dev/null ||
  fail 'manual-page generator does not bind deterministic wrapping'
grep -F -- '--no-highlight' "$root/tools/update-man-pages.sh" >/dev/null ||
  fail 'manual-page generator does not disable writer syntax highlighting'
grep -F 'canonicalize-man-roff.awk' "$root/tools/update-man-pages.sh" >/dev/null ||
  fail 'manual-page generator does not canonicalize Pandoc writer output'
grep -F 'Pandoc 3.1 through 3.x is required' \
  "$root/tools/update-man-pages.sh" >/dev/null ||
  fail 'manual-page generator does not bind the qualified Pandoc major series'
if grep -R -E 'scdoc|\.scd([^A-Za-z0-9_]|$)' \
    "$root/docs/man" "$root/meson.options" \
    "$root/.github/workflows/ci.yml" >/dev/null; then
  fail 'retired scdoc build or source references remain'
fi
grep -F 'libpkgsource-plan/candidate-control/v1' \
  "$root/src/internal/candidate_control_identity.cpp" >/dev/null ||
  fail 'first public control identity domain is not version one'
grep -F 'libpkgsource-plan/candidate-control/v1' \
  "$root/docs/protocols/candidate-control-identity-v1.md" >/dev/null ||
  fail 'candidate-control identity specification is missing'
grep -F '2064db1e0c8a2934b1998aae9cd289cf4faace757dbc0f153afd017a85781795' \
  "$root/docs/protocols/candidate-control-identity-v1.md" >/dev/null ||
  fail 'candidate-control fixed vector is missing'
if grep -R -E 'candidate-control/v2|source_syntax|recipe_yaml_v[0-9]' \
    "$root" --exclude-dir=.git --exclude='check_release_contract.sh' >/dev/null; then
  fail 'pre-release source or identity generations remain in release tree'
fi
