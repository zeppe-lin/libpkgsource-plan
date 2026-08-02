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
  docs/architecture.md \
  docs/abi.md \
  docs/protocols/candidate-control-identity-v1.md \
  docs/testing.md \
  docs/code-style.md \
  docs/manpage-markdown.md \
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
require_heading "$root/docs/architecture.md" '## Authority boundary'
require_heading "$root/docs/abi.md" '## Canonical manifest'
require_heading "$root/docs/abi.md" '## Versioning'
require_heading "$root/docs/abi.md" '## Qualification'
require_heading "$root/docs/architecture.md" '## Projection map'
require_heading "$root/docs/architecture.md" '## Installed documentation'
require_heading "$root/docs/architecture.md" '## Repository layout'
require_heading "$root/docs/architecture.md" '## Projection pipeline'
require_heading "$root/docs/architecture.md" '## SHA-256 provider boundary'
require_heading "$root/docs/architecture.md" '## Excluded source authority'
require_heading "$root/docs/protocols/candidate-control-identity-v1.md" '## Canonical record'
require_heading "$root/docs/protocols/candidate-control-identity-v1.md" '## Fixed vector'
require_heading "$root/docs/testing.md" '## Projection behavior'
require_heading "$root/docs/testing.md" '## Internal identity behavior'
require_heading "$root/docs/testing.md" '## Release qualification'
require_heading "$root/docs/manpage-markdown.md" '## Conversion contract'
require_heading "$root/docs/manpage-markdown.md" '## Forbidden Markdown'
require_heading "$root/docs/history/in-tree-adapter-migration.md" '## No compatibility layer'
require_heading "$root/CONTRIBUTING.md" '## Boundary first'
require_heading "$root/MAINTAINING.md" '## Release checklist'
require_heading "$root/HISTORY.md" '## 1.0.0'

for file in "$root/README.md" "$root/docs/architecture.md" \
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

for retired in DESIGN.md CODESTYLE.md TESTING.md MANPAGE-MARKDOWN.md \
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
