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
  docs/abi.md \
  docs/protocols/candidate-control-identity-v1.md \
  TESTING.md \
  docs/code-style.md \
  docs/manpage-markdown.md \
  docs/html.md \
  docs/history/in-tree-adapter-migration.md \
  CONTRIBUTING.md \
  MAINTAINING.md \
  HISTORY.md
do
  [ -s "$root/$document" ] || fail "$document is missing or empty"
done

require_heading "$root/README.md" '# libpkgsource-plan'
require_heading "$root/README.md" '## Projection'
require_heading "$root/README.md" '## Public API'
require_heading "$root/README.md" '## Installed documentation'
require_heading "$root/README.md" '## HTML documentation'
require_heading "$root/DESIGN.md" '## Authority boundary'
require_heading "$root/docs/abi.md" '## Canonical manifest'
require_heading "$root/docs/abi.md" '## Versioning'
require_heading "$root/docs/abi.md" '## Qualification'
require_heading "$root/DESIGN.md" '## Projection map'
require_heading "$root/DESIGN.md" '## Installed documentation'
require_heading "$root/DESIGN.md" '## HTML publication boundary'
require_heading "$root/DESIGN.md" '## Repository layout'
require_heading "$root/DESIGN.md" '## Projection pipeline'
require_heading "$root/DESIGN.md" '## SHA-256 provider boundary'
require_heading "$root/DESIGN.md" '## Excluded source authority'
require_heading "$root/DESIGN.md" '## Failure model'
require_heading "$root/docs/protocols/candidate-control-identity-v1.md" '## Canonical record'
require_heading "$root/docs/protocols/candidate-control-identity-v1.md" '## Fixed vector'
require_heading "$root/TESTING.md" '## Unit projection behavior'
require_heading "$root/TESTING.md" '## Integration seams'
require_heading "$root/TESTING.md" '## Protocol and mechanism behavior'
require_heading "$root/TESTING.md" '## Release qualification'
require_heading "$root/docs/manpage-markdown.md" '## Conversion contract'
require_heading "$root/docs/manpage-markdown.md" '## Forbidden Markdown'
require_heading "$root/docs/html.md" '## Output layout'
require_heading "$root/docs/html.md" '## Installation'
require_heading "$root/docs/html.md" '## Site publication'
require_heading "$root/docs/history/in-tree-adapter-migration.md" '## No compatibility layer'
require_heading "$root/CONTRIBUTING.md" '## Boundary first'
require_heading "$root/MAINTAINING.md" '## Release checklist'
require_heading "$root/HISTORY.md" '## 1.1.0'
require_heading "$root/HISTORY.md" '## 1.0.0'

for file in "$root/README.md" "$root/DESIGN.md" \
            "$root/docs/history/in-tree-adapter-migration.md" "$root/docs/man/pkgsource_plan_adapter.3.md"; do
  grep -F '<libpkgsource-plan/libpkgsource-plan.h>' "$file" >/dev/null ||
    fail "${file#$root/} omits the umbrella include"
done

for section in '# DESCRIPTION' '# OWNERSHIP' '# IDENTITIES' '# ERRORS' \
               '# AUTHORITY' '# ABI' '# SEE ALSO'; do
  grep -F "$section" "$root/docs/man/pkgsource_plan_adapter.3.md" >/dev/null ||
    fail "manual omits section: $section"
done

grep -F '2064db1e0c8a2934b1998aae9cd289cf' \
  "$root/docs/protocols/candidate-control-identity-v1.md" >/dev/null ||
  fail 'identity specification omits the fixed vector'

for duplicate in docs/architecture.md docs/testing.md
do
  [ ! -e "$root/$duplicate" ] || fail "duplicate documentation authority remains: $duplicate"
done

for retired in CODESTYLE.md MANPAGE-MARKDOWN.md \
               CANDIDATE-CONTROL-IDENTITY-1.md MIGRATION.md
do
  [ ! -e "$root/$retired" ] ||
    fail "retired root document remains: $retired"
done

if grep -R -E 'parse_(recipe|profiles)|seal_recipe_yaml|yaml_parser' \
    "$root"/*.md "$root/docs" "$root/docs/man" >/dev/null; then
  fail 'source-syntax implementation appears in adapter documentation'
fi

if grep -R -E 'is thread[- ]safe|is lock[- ]free|is backward compatible' \
    "$root"/*.md "$root/docs" "$root/docs/man" >/dev/null; then
  fail 'documentation claims an unqualified concurrency or compatibility rule'
fi
