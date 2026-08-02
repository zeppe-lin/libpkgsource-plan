// SPDX-FileCopyrightText: 2026 Alexandr Savca
// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include <stdexcept>

namespace pkgsource::plan_adapter::internal {

/// Failure while importing or constructing identity material.
class identity_error final : public std::runtime_error {
public:
  using std::runtime_error::runtime_error;
};

} // namespace pkgsource::plan_adapter::internal
