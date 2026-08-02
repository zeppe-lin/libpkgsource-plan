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
grep -F "version: '>=3.0.0'" "$root/meson.build" >/dev/null ||
  fail 'libpkgsource dependency floor is not 3.0.0'
grep -F "version: '>=0.2.0'" "$root/meson.build" >/dev/null ||
  fail 'libpkgplan dependency floor is not 0.2.0'
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
grep -F 'libpkgsource-plan/candidate-control/v1' \
  "$root/src/adapter.cpp" >/dev/null ||
  fail 'first public control identity domain is not version one'
grep -F 'libpkgsource-plan/candidate-control/v1' \
  "$root/CANDIDATE-CONTROL-IDENTITY-1.md" >/dev/null ||
  fail 'candidate-control identity specification is missing'
grep -F '2064db1e0c8a2934b1998aae9cd289cf4faace757dbc0f153afd017a85781795' \
  "$root/CANDIDATE-CONTROL-IDENTITY-1.md" >/dev/null ||
  fail 'candidate-control fixed vector is missing'
if grep -R -E 'candidate-control/v2|source_syntax|recipe_yaml_v[0-9]' \
    "$root" --exclude-dir=.git --exclude='check_release_contract.sh' >/dev/null; then
  fail 'pre-release source or identity generations remain in release tree'
fi
