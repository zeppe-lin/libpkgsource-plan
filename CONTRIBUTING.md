<!-- SPDX-FileCopyrightText: 2026 Alexandr Savca -->
<!-- SPDX-License-Identifier: GPL-3.0-or-later -->

# Contributing

This repository owns one projection boundary: sealed `libpkgsource` authority to
facts already representable by `libpkgplan`. It must not parse recipes, resolve
requirements, execute programs, invent planner policy, or widen either owner
model.

Every projected field needs a positive test. Every intentionally excluded source
field needs a stability test proving it does not alter projected control.
Conversely, every projected field must alter the adapter-owned control identity
when its value changes.

Changes require the exact authoritative dependency bundles or signed tags for
both `libpkgsource` and `libpkgplan`. Do not reconstruct either API from reports,
old patches, or remembered declarations.
