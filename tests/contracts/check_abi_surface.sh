#!/bin/sh
# SPDX-FileCopyrightText: 2026 Alexandr Savca
# SPDX-License-Identifier: GPL-3.0-or-later
set -eu

nm_tool=$1
library=$2

fail()
{
  echo "abi-surface-test: $*" >&2
  exit 1
}

raw=$($nm_tool -D --defined-only "$library") ||
  fail 'cannot inspect shared-library dynamic symbols'
demangled=$($nm_tool -D --defined-only -C "$library") ||
  fail 'cannot demangle shared-library dynamic symbols'

for required in \
  'pkgsource::plan_adapter::projection_error::projection_error' \
  'pkgsource::plan_adapter::projection_error::code() const' \
  'pkgsource::plan_adapter::candidate_projection::candidate_projection' \
  'pkgsource::plan_adapter::candidate_projection::source() const' \
  'pkgsource::plan_adapter::candidate_projection::source_identity() const' \
  'pkgsource::plan_adapter::candidate_projection::candidate() const' \
  'pkgsource::plan_adapter::project_candidate'
do
  printf '%s\n' "$demangled" | grep -F "$required" >/dev/null ||
    fail "public symbol is missing: $required"
done

printf '%s\n' "$raw" | grep -F 'LIBPKGSOURCE_PLAN_1.0' >/dev/null ||
  fail 'ABI version node is missing'

if printf '%s\n' "$raw" | grep -E \
    '(_ZNK?St|_ZN9__gnu_cxx|_ZNK?9pkgsource12plan_adapter8internal)' \
    >/dev/null; then
  fail 'private implementation or standard-library symbols escaped the ABI'
fi
