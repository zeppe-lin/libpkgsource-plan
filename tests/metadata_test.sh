#!/bin/sh
# SPDX-FileCopyrightText: 2026 Alexandr Savca
# SPDX-License-Identifier: GPL-3.0-or-later
set -eu
build_root=$1
project_version=$2
metadata=$build_root/meson-private/libpkgsource-plan.pc
[ -s "$metadata" ] || metadata=$(
  find "$build_root" -type f -name libpkgsource-plan.pc -print | sed -n '1p'
)
[ -n "${metadata:-}" ] && [ -s "$metadata" ] || {
  echo 'plan-metadata-test: generated metadata not found' >&2
  exit 1
}

grep -F 'Name: libpkgsource-plan' "$metadata" >/dev/null
grep -F "Version: $project_version" "$metadata" >/dev/null
for requirement in 'libpkgsource >= 3.0.0' 'libpkgplan >= 0.2.0'; do
  count=$(grep '^Requires:' "$metadata" | grep -oF "$requirement" | wc -l)
  [ "$count" -eq 1 ] || {
    echo "plan-metadata-test: expected one '$requirement', found $count" >&2
    cat "$metadata" >&2
    exit 1
  }
done
count=$(grep '^Requires.private:' "$metadata" | grep -oF 'libcrypto' | wc -l)
[ "$count" -eq 1 ] || {
  echo "plan-metadata-test: expected one private libcrypto, found $count" >&2
  cat "$metadata" >&2
  exit 1
}
grep -E 'Libs:.*-lpkgsource-plan' "$metadata" >/dev/null
