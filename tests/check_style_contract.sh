#!/bin/sh
# SPDX-FileCopyrightText: 2026 Alexandr Savca
# SPDX-License-Identifier: GPL-3.0-or-later
set -eu

root=$1

fail()
{
  echo "style-contract-test: $*" >&2
  exit 1
}

for file in .clang-format .editorconfig CODESTYLE.md; do
  [ -s "$root/$file" ] || fail "$file is missing or empty"
done

markdown=$(find "$root" -maxdepth 1 -type f -name '*.md' -print)

if grep -n -E 'SPDX-(FileCopyrightText|License-Identifier)' $markdown >/dev/null; then
  fail 'Markdown contains SPDX comments; use COPYING and COPYRIGHT'
fi

if grep -n -E '^(===+|---+)[[:space:]]*$' $markdown >/dev/null; then
  fail 'Markdown contains Setext headings or horizontal rules'
fi

if grep -n "$(printf '\t')" $markdown >/dev/null; then
  fail 'Markdown contains tab indentation'
fi

if grep -n -E '[[:blank:]]+$' $markdown >/dev/null; then
  fail 'Markdown contains trailing whitespace'
fi

grep -F '# Code style' "$root/CODESTYLE.md" >/dev/null ||
  fail 'CODESTYLE.md does not declare its purpose'
grep -F 'Control statements always use braces' "$root/CODESTYLE.md" >/dev/null ||
  fail 'CODESTYLE.md does not bind braced control flow'
grep -F 'Markdown uses ATX headings only' "$root/CODESTYLE.md" >/dev/null ||
  fail 'CODESTYLE.md does not bind Markdown headings'
