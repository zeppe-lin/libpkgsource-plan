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
grep -F "soversion: '1'" "$root/src/meson.build" >/dev/null ||
  fail 'library SONAME generation is not 1'
grep -F "version: '>=3.0.0'" "$root/meson.build" >/dev/null ||
  fail 'libpkgsource dependency floor is not 3.0.0'
grep -F "version: '>=0.2.0'" "$root/meson.build" >/dev/null ||
  fail 'libpkgplan dependency floor is not 0.2.0'
grep -F 'libpkgsource-plan/candidate-control/v1' \
  "$root/src/adapter.cpp" >/dev/null ||
  fail 'first public control identity domain is not version one'
if grep -R -E 'candidate-control/v2|source_syntax|recipe_yaml_v[0-9]' \
    "$root" --exclude-dir=.git --exclude='check_release_contract.sh' >/dev/null; then
  fail 'pre-release source or identity generations remain in release tree'
fi
