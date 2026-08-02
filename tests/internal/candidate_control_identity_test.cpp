// SPDX-FileCopyrightText: 2026 Alexandr Savca
// SPDX-License-Identifier: GPL-3.0-or-later
#include "../../src/internal/candidate_control_identity.h"
#include "../support/test_support.h"

#include <libpkgplan/control.h>

#include <string>
#include <vector>

namespace {

using pkgsource::plan_adapter::internal::compute_candidate_control_identity;
using test_support::require_equal;

[[nodiscard]] pkgplan::candidate_control_projection published_control()
{
  return pkgplan::candidate_control_projection(
      {pkgplan::runtime_dependency_declaration::make("libfoo")},
      {
          pkgplan::removal_lifecycle_declaration::make(
              pkgplan::removal_lifecycle_phase::pre_remove,
              "text/x-posix-shell",
              "echo pre-remove\n"),
          pkgplan::removal_lifecycle_declaration::make(
              pkgplan::removal_lifecycle_phase::post_remove,
              "text/x-posix-shell",
              "echo post-remove\n"),
      },
      {pkgplan::target_profile_fact::make("pkgsource.target-architectures",
                                          "x86_64")});
}

void binds_published_protocol_vector()
{
  require_equal(
      compute_candidate_control_identity(published_control()).string(),
      std::string("v1:sha256:"
                  "2064db1e0c8a2934b1998aae9cd289cf"
                  "4faace757dbc0f153afd017a85781795"),
      "identity encoder must bind the published candidate-control/v1 vector");
}

void binds_planner_normalized_order()
{
  const pkgplan::candidate_control_projection control(
      {
          pkgplan::runtime_dependency_declaration::make("libz"),
          pkgplan::runtime_dependency_declaration::make("liba"),
      },
      {},
      {});

  const pkgplan::candidate_control_projection canonical(
      {
          pkgplan::runtime_dependency_declaration::make("liba"),
          pkgplan::runtime_dependency_declaration::make("libz"),
      },
      {},
      {});

  require_equal(compute_candidate_control_identity(control),
                compute_candidate_control_identity(canonical),
                "identity must observe planner-normalized control order");
}

} // namespace

int main()
{
  return test_support::run({
      {"binds published protocol vector", binds_published_protocol_vector},
      {"binds planner normalized order", binds_planner_normalized_order},
  });
}
