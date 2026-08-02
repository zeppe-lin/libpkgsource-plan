#!/bin/sh
# SPDX-FileCopyrightText: 2026 Alexandr Savca
# SPDX-License-Identifier: GPL-3.0-or-later
set -eu

prefix=$1
project=libpkgsource-plan
docdir=$prefix/share/doc/$project
mandir=$prefix/share/man/man3

fail()
{
  echo "installed-docs-test: $*" >&2
  exit 1
}

for file in \
  README.md \
  HISTORY.md \
  CONTRIBUTING.md \
  MAINTAINING.md \
  COPYING \
  COPYRIGHT \
  architecture.md \
  abi.md \
  code-style.md \
  manpage-markdown.md \
  testing.md \
  protocols/candidate-control-identity-v1.md \
  history/in-tree-adapter-migration.md \
  man/pkgsource_plan_adapter.3.md
do
  [ -s "$docdir/$file" ] || fail "missing installed documentation: $file"
done

[ -s "$mandir/pkgsource_plan_adapter.3" ] ||
  fail 'generated manual page is not installed in man3'

if find "$docdir" -type f \( -name meson.build -o -path '*/generated/*' \) |
    grep . >/dev/null; then
  fail 'build metadata or derived roff escaped into canonical documentation'
fi
