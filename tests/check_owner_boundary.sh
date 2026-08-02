#!/bin/sh
# SPDX-FileCopyrightText: 2026 Alexandr Savca
# SPDX-License-Identifier: GPL-3.0-or-later
set -eu

root=$1

fail()
{
  echo "owner-boundary-test: $*" >&2
  exit 1
}

public_header=$root/include/libpkgsource-plan/adapter.h
implementation=$root/src/adapter.cpp
build=$root/meson.build

for required in \
  '<libpkgsource/snapshot.h>' \
  '<libpkgplan/package_fact.h>'
do
  grep -F "$required" "$public_header" >/dev/null ||
    fail "public API omits owner header $required"
done

grep -F "dependency(" "$build" >/dev/null ||
  fail 'project does not declare external dependencies'
grep -F "'libpkgsource'" "$build" >/dev/null ||
  fail 'libpkgsource owner dependency is missing'
grep -F "'libpkgplan'" "$build" >/dev/null ||
  fail 'libpkgplan owner dependency is missing'
grep -F "'libcrypto'" "$build" >/dev/null ||
  fail 'direct identity backend dependency is missing'

if grep -R -E '#include <(yaml|libpkgsource-codec|libpkgimage)' \
    "$root/include" "$root/src" >/dev/null; then
  fail 'foreign parser, codec, or image authority crosses adapter source'
fi

if grep -R -E 'dependency\(.?(yaml|libpkgsource-codec|libpkgimage)' \
    "$root/meson.build" "$root/src/meson.build" >/dev/null; then
  fail 'foreign parser, codec, or image dependency crosses adapter build'
fi

if grep -R -E 'parse_(recipe|profiles)|seal_recipe_yaml|yaml_parser' \
    "$root/include" "$implementation" >/dev/null; then
  fail 'source-syntax parsing crosses the planner adapter boundary'
fi

grep -F 'pkgsource::source_snapshot source_' "$public_header" >/dev/null ||
  fail 'projection does not retain complete source authority'
