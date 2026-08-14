#!/bin/sh
# SPDX-FileCopyrightText: 2026 Alexandr Savca
# SPDX-License-Identifier: GPL-3.0-or-later
set -eu
build_root=$1
project_version=$2
metadata=$build_root/meson-private/libpkgsource-plan.pc
[ -s "$metadata" ] || metadata=$(
  find "$build_root" -type f -name libpkgsource-plan.pc -print |
    sed -n '1p'
)
[ -n "${metadata:-}" ] && [ -s "$metadata" ] || {
  echo 'plan-metadata-test: generated metadata not found' >&2
  exit 1
}

fail()
{
  echo "plan-metadata-test: $*" >&2
  cat "$metadata" >&2
  exit 1
}

field_count()
{
  field=$1
  awk -v field="$field:" '
    $1 == field { ++count }
    END { print count + 0 }
  ' "$metadata"
}

module_count()
{
  field=$1
  module=$2
  awk -v field="$field:" -v module="$module" '
    $1 == field {
      for (i = 2; i <= NF; ++i) {
        token = $i
        sub(/,$/, "", token)
        if (token == module) ++count
      }
    }
    END { print count + 0 }
  ' "$metadata"
}

expect_count()
{
  field=$1
  module=$2
  expected=$3
  actual=$(module_count "$field" "$module")
  [ "$actual" -eq "$expected" ] ||
    fail "expected $module $expected time(s) in $field, found $actual"
}

grep -F 'Name: libpkgsource-plan' "$metadata" >/dev/null ||
  fail 'module name is missing'
grep -F "Version: $project_version" "$metadata" >/dev/null ||
  fail "module version is not $project_version"

[ "$(field_count Requires)" -eq 1 ] ||
  fail 'expected exactly one Requires field'
[ "$(field_count Requires.private)" -eq 1 ] ||
  fail 'expected exactly one Requires.private field'

expect_count Requires libpkgsource 2 # >= X, and < Y
expect_count Requires libpkgplan 1
expect_count Requires libcrypto 0
expect_count Requires.private libpkgsource 0
expect_count Requires.private libpkgplan 0
expect_count Requires.private libcrypto 1

grep -E \
  'Requires:.*libpkgsource[[:space:]]*>=[[:space:]]*4\.0\.0' \
  "$metadata" >/dev/null ||
  fail 'public libpkgsource floor is not 4.0.0'
grep -E \
  'Requires:.*libpkgplan[[:space:]]*>=[[:space:]]*0\.3\.1' \
  "$metadata" >/dev/null ||
  fail 'public libpkgplan floor is not 0.3.1'
grep -E 'Requires\.private:.*libcrypto' "$metadata" >/dev/null ||
  fail 'private libcrypto requirement is missing'
grep -E 'Libs:.*-lpkgsource-plan' "$metadata" >/dev/null ||
  fail 'library linker flag is missing'
