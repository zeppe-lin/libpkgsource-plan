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

require_heading()
{
  file=$1
  heading=$2
  grep -F -- "$heading" "$file" >/dev/null ||
    fail "${file#$root/} omits heading: $heading"
}

for document in \
  README.md \
  DESIGN.md \
  CANDIDATE-CONTROL-IDENTITY-1.md \
  TESTING.md \
  CODESTYLE.md \
  MANPAGE-MARKDOWN.md \
  MIGRATION.md \
  CONTRIBUTING.md \
  MAINTAINING.md \
  HISTORY.md
do
  [ -s "$root/$document" ] || fail "$document is missing or empty"
done

require_heading "$root/README.md" '# libpkgsource-plan'
require_heading "$root/README.md" '## Projection'
require_heading "$root/README.md" '## Public API'
require_heading "$root/DESIGN.md" '## Authority boundary'
require_heading "$root/DESIGN.md" '## Projection map'
require_heading "$root/DESIGN.md" '## Excluded source authority'
require_heading "$root/CANDIDATE-CONTROL-IDENTITY-1.md" '## Canonical record'
require_heading "$root/CANDIDATE-CONTROL-IDENTITY-1.md" '## Fixed vector'
require_heading "$root/TESTING.md" '## Executable behavior'
require_heading "$root/TESTING.md" '## Release qualification'
require_heading "$root/MANPAGE-MARKDOWN.md" '## Conversion contract'
require_heading "$root/MANPAGE-MARKDOWN.md" '## Forbidden Markdown'
require_heading "$root/MIGRATION.md" '## No compatibility layer'
require_heading "$root/CONTRIBUTING.md" '## Boundary first'
require_heading "$root/MAINTAINING.md" '## Release checklist'
require_heading "$root/HISTORY.md" '## 1.0.0'

for file in "$root/README.md" "$root/DESIGN.md" \
            "$root/MIGRATION.md" "$root/man/pkgsource_plan_adapter.3.md"; do
  grep -F '<libpkgsource-plan/libpkgsource-plan.h>' "$file" >/dev/null ||
    fail "${file#$root/} omits the umbrella include"
done

for section in '# DESCRIPTION' '# OWNERSHIP' '# IDENTITIES' '# ERRORS' \
               '# AUTHORITY' '# ABI' '# SEE ALSO'; do
  grep -F "$section" "$root/man/pkgsource_plan_adapter.3.md" >/dev/null ||
    fail "manual omits section: $section"
done

grep -F '2064db1e0c8a2934b1998aae9cd289cf' \
  "$root/CANDIDATE-CONTROL-IDENTITY-1.md" >/dev/null ||
  fail 'identity specification omits the fixed vector'

if grep -R -E 'parse_(recipe|profiles)|seal_recipe_yaml|yaml_parser' \
    "$root"/*.md "$root/man" >/dev/null; then
  fail 'source-syntax implementation appears in adapter documentation'
fi

if grep -R -E 'is thread[- ]safe|is lock[- ]free|is backward compatible' \
    "$root"/*.md "$root/man" >/dev/null; then
  fail 'documentation claims an unqualified concurrency or compatibility rule'
fi
