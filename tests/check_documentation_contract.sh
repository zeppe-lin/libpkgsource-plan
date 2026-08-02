#!/bin/sh
# SPDX-FileCopyrightText: 2026 Alexandr Savca
# SPDX-License-Identifier: GPL-3.0-or-later
set -eu
root=$1
fail()
{
  echo "documentation-contract-test: $*" >&2
  exit 1
}
require()
{
  file=$1
  text=$2
  grep -F -- "$text" "$file" >/dev/null ||
    fail "${file#$root/} omits: $text"
}

for file in README.md DESIGN.md TESTING.md MIGRATION.md CONTRIBUTING.md MAINTAINING.md HISTORY.md CANDIDATE-CONTROL-IDENTITY-1.md; do
  [ -s "$root/$file" ] || fail "$file is missing or empty"
done

require "$root/README.md" 'narrow composition adapter'
require "$root/DESIGN.md" 'Excluded facts'
require "$root/CANDIDATE-CONTROL-IDENTITY-1.md" '2064db1e0c8a2934b1998aae9cd289cf'
require "$root/TESTING.md" 'Required release matrix'
require "$root/MIGRATION.md" 'No compatibility layer'
require "$root/CONTRIBUTING.md" 'Every intentionally excluded source'
require "$root/MAINTAINING.md" 'candidate-control/v1'
require "$root/HISTORY.md" 'First independent source-to-planner projection release.'
require "$root/man/pkgsource_plan_adapter.3.scd" '# AUTHORITY'

if grep -R -E 'parse_(recipe|profiles)|yaml_parser|seal_recipe_yaml' \
    "$root/README.md" "$root/DESIGN.md" "$root/man" >/dev/null; then
  fail 'syntax parsing appears in planner-adapter documentation'
fi
