// SPDX-FileCopyrightText: 2026 Alexandr Savca
// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include <libpkgplan/digest.h>
#include <libpkgsource/identity.h>

namespace pkgsource::plan_adapter::internal {

/// Import the source-owned package-release digest into the planner domain.
[[nodiscard]] pkgplan::package_release_identity import_package_release_identity(
    const pkgsource::package_release_identity& identity);

} // namespace pkgsource::plan_adapter::internal
