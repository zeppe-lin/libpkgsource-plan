// SPDX-FileCopyrightText: 2026 Alexandr Savca
// SPDX-License-Identifier: GPL-3.0-or-later
#include "../../src/internal/source_release_identity.h"
#include "../support/test_support.h"

#include <libpkgsource/identity.h>

#include <string>

namespace {

using pkgsource::plan_adapter::internal::import_package_release_identity;
using test_support::require_equal;

void imports_exact_digest_bytes()
{
  const std::string hexadecimal("0123456789abcdef0123456789abcdef"
                                "0123456789abcdef0123456789abcdef");
  const pkgsource::package_release_identity source =
      pkgsource::package_release_identity::from_sha256(hexadecimal);

  require_equal(import_package_release_identity(source).string(),
                std::string("v1:sha256:") + hexadecimal,
                "source digest bytes must enter the planner domain unchanged");
}

} // namespace

int main()
{
  return test_support::run({
      {"imports exact digest bytes", imports_exact_digest_bytes},
  });
}
