#!/bin/sh
# SPDX-FileCopyrightText: 2026 Alexandr Savca
# SPDX-License-Identifier: GPL-3.0-or-later
set -eu
root=$1
source=$root/src/adapter.cpp
fail()
{
  echo "projection-boundary-test: $*" >&2
  exit 1
}

grep -F 'recipe.run_requirements()' "$source" >/dev/null ||
  fail 'runtime requirement projection is missing'
grep -F 'lifecycle_action::pre_remove' "$source" >/dev/null ||
  fail 'pre-remove projection is missing'
grep -F 'lifecycle_action::post_remove' "$source" >/dev/null ||
  fail 'post-remove projection is missing'
grep -F 'recipe.architectures().target()' "$source" >/dev/null ||
  fail 'target architecture projection is missing'
grep -F 'recipe.release()' "$source" >/dev/null ||
  fail 'package release projection is missing'

for forbidden in \
  'build_requirements()' \
  'check_requirements()' \
  'selected_build_profiles()' \
  'architectures().build()' \
  'recipe.sources()' \
  'recipe.build_program()' \
  'lifecycle_action::pre_install,' \
  'lifecycle_action::post_install,'
do
  if grep -F "$forbidden" "$source" >/dev/null; then
    fail "forbidden source fact crosses planner boundary: $forbidden"
  fi
done

grep -F 'source_snapshot source_' \
  "$root/include/libpkgsource-plan/adapter.h" >/dev/null ||
  fail 'projection does not retain complete source provenance'
