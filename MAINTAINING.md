<!-- SPDX-FileCopyrightText: 2026 Alexandr Savca -->
<!-- SPDX-License-Identifier: GPL-3.0-or-later -->

# Maintaining

The adapter has an independent project version and SONAME. Release only after
building and testing against the signed minimum and current supported tags of
both owner libraries.

Before tagging, qualify GCC and Clang shared/static builds, ASan/UBSan, generated
pkg-config metadata, installed public-header consumers, shared-object dependency
closure, manuals, and exact patch replay.

The `libpkgsource-plan/candidate-control/v1` digest material is adapter-owned;
its typed value is owned by `libpkgplan`. Any change to projected field
participation, ordering, or digest framing requires an explicit identity-version
decision and fixed vectors. Do not use package-release coordinates, source origin,
build/check-only requirements, or unrelated lifecycle material as planner
candidate authority unless `libpkgplan` first establishes a corresponding owner
field.
