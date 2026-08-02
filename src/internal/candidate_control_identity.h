// SPDX-FileCopyrightText: 2026 Alexandr Savca
// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include <libpkgplan/control.h>
#include <libpkgplan/digest.h>

namespace pkgsource::plan_adapter::internal {

/// Compute the published candidate-control/v1 identity.
[[nodiscard]] pkgplan::candidate_control_identity
compute_candidate_control_identity(
    const pkgplan::candidate_control_projection& control);

} // namespace pkgsource::plan_adapter::internal
