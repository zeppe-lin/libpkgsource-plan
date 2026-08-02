#!/bin/sh
# SPDX-FileCopyrightText: 2026 Alexandr Savca
# SPDX-License-Identifier: GPL-3.0-or-later
set -eu

formatter=$1
root=$2

find "$root/include" "$root/src" "$root/tests" \
  -type f \( -name '*.h' -o -name '*.cpp' \) -print0 |
  sort -z |
  xargs -0 "$formatter" --dry-run --Werror
