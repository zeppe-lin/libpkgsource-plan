#!/bin/sh
# SPDX-FileCopyrightText: 2026 Alexandr Savca
# SPDX-License-Identifier: GPL-3.0-or-later
set -eu

usage()
{
  echo "usage: $0 --check|--write [pandoc] [source-root]" >&2
  exit 2
}

fail()
{
  echo "update-man-pages: $*" >&2
  exit 1
}

[ "$#" -ge 1 ] && [ "$#" -le 3 ] || usage
mode=$1
pandoc=${2:-pandoc}
root=${3:-$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)}

case $mode in
  --check | --write)
    ;;
  *)
    usage
    ;;
esac

command -v "$pandoc" >/dev/null 2>&1 || fail "Pandoc not found: $pandoc"

version=$($pandoc --version | sed -n '1s/^pandoc //p')
[ -n "$version" ] || fail 'cannot determine Pandoc version'
major=${version%%.*}
remainder=${version#*.}
minor=${remainder%%.*}
case $major:$minor in
  *[!0-9:]* | :* | *:)
    fail "cannot parse Pandoc version: $version"
    ;;
esac
if [ "$major" -lt 3 ] || { [ "$major" -eq 3 ] && [ "$minor" -lt 1 ]; }; then
  fail "Pandoc 3.1 or later is required; found $version"
fi

source=$root/man/pkgsource_plan_adapter.3.md
output=$root/man/generated/pkgsource_plan_adapter.3
[ -f "$source" ] || fail "missing source: ${source#$root/}"
mkdir -p "$(dirname -- "$output")"

raw=$(mktemp)
temporary=$(mktemp)
trap 'rm -f "$raw" "$temporary"' EXIT HUP INT TERM

"$pandoc" \
  --from=markdown-smart \
  --to=man \
  --standalone \
  --fail-if-warnings \
  --eol=lf \
  --wrap=none \
  "$source" > "$raw"

printf '.\\" Generated from man/pkgsource_plan_adapter.3.md; do not edit.\n' \
  > "$temporary"
sed '1d' "$raw" >> "$temporary"

case $mode in
  --write)
    cat "$temporary" > "$output"
    ;;
  --check)
    [ -f "$output" ] || fail "missing generated page: ${output#$root/}"
    if ! cmp -s "$temporary" "$output"; then
      diff -u "$output" "$temporary" || true
      fail 'generated manual page is stale'
    fi
    ;;
esac
