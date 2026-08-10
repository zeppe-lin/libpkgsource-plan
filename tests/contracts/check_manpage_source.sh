#!/bin/sh
# SPDX-FileCopyrightText: 2026 Alexandr Savca
# SPDX-License-Identifier: GPL-3.0-or-later
set -eu

root=$1
source=$root/docs/man/pkgsource_plan_adapter.3.md

fail()
{
  echo "manpage-source-test: $*" >&2
  exit 1
}

[ -s "$source" ] || fail 'canonical Markdown source is missing or empty'

expected_title='% PKGSOURCE_PLAN_ADAPTER(3) libpkgsource-plan 1.1.0 | libpkgsource-plan'
actual_title=$(sed -n '1p' "$source")
[ "$actual_title" = "$expected_title" ] ||
  fail 'Pandoc title block does not match the release page identity'

expected_sections='NAME
SYNOPSIS
DESCRIPTION
OWNERSHIP
IDENTITIES
ERRORS
AUTHORITY
ABI
SEE ALSO'
actual_sections=$(sed -n 's/^# \([A-Z][A-Z ]*\)$/\1/p' "$source")
[ "$actual_sections" = "$expected_sections" ] || {
  echo '--- expected sections ---' >&2
  printf '%s\n' "$expected_sections" >&2
  echo '--- actual sections ---' >&2
  printf '%s\n' "$actual_sections" >&2
  fail 'manual sections are missing, renamed, or out of order'
}

name_line=$(awk '
  /^# NAME$/ { in_name = 1; next }
  /^# / && in_name { exit }
  in_name && NF { print; exit }
' "$source")
[ "$name_line" = \
  'pkgsource_plan_adapter - project sealed source authority into planner facts' ] ||
  fail 'NAME section does not contain the canonical name and purpose'

grep -F '```cpp' "$source" >/dev/null ||
  fail 'SYNOPSIS is not an explicit C++ fenced block'
grep -F '#include <libpkgsource-plan/libpkgsource-plan.h>' "$source" >/dev/null ||
  fail 'SYNOPSIS omits the umbrella header'

if grep -n -E '^(===+|---+)[[:space:]]*$' "$source" >/dev/null; then
  fail 'manual source contains a Setext heading or horizontal rule'
fi
if grep -n -E "^[.'](TH|SH|SS|TP|IP|PP|RS|RE|EX|EE)([[:space:]]|$)" \
    "$source" >/dev/null; then
  fail 'manual source contains raw roff'
fi
if grep -n -E '^[[:space:]]*</?[A-Za-z][^>]*>' "$source" >/dev/null; then
  fail 'manual source contains raw HTML'
fi
if grep -n -E '^\|.*\|[[:space:]]*$|^\[[^]]+\]:|\]\([^)]*\)|\[\^[^]]+\]' \
    "$source" >/dev/null; then
  fail 'manual source contains a forbidden table, link, or footnote construct'
fi
if grep -n -E '^[[:space:]]*[-*] \[[ xX]\][[:space:]]' "$source" >/dev/null; then
  fail 'manual source contains a task list'
fi
if grep -n "$(printf '\t')" "$source" >/dev/null; then
  fail 'manual source contains tabs'
fi
if grep -n -E '[[:blank:]]+$' "$source" >/dev/null; then
  fail 'manual source contains trailing whitespace'
fi
