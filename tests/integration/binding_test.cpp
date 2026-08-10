// SPDX-FileCopyrightText: 2026 Alexandr Savca
// SPDX-License-Identifier: GPL-3.0-or-later
#include <libpkgsource-plan/libpkgsource-plan.h>

#include "../support/source_fixture.h"
#include "../support/test_support.h"

#include <string>
#include <utility>

namespace {

using pkgsource::plan_adapter::candidate_projection;
using pkgsource::plan_adapter::project_candidate;
using pkgsource::plan_adapter::projection_error;
using pkgsource::plan_adapter::projection_error_code;
using test_support::require;
using test_support::require_equal;

void reject_mismatched_control_binding()
{
  candidate_projection baseline =
      project_candidate(source_fixture::make_snapshot());

  source_fixture::options options;
  options.run_dependency = "libbar";
  pkgsource::source_snapshot changed =
      source_fixture::make_snapshot(std::move(options));

  try {
    candidate_projection forged(std::move(changed), baseline.candidate());
    static_cast<void>(forged);
  } catch (const projection_error& error) {
    require(error.code() == projection_error_code::source_binding,
            "control mismatch must report source binding failure");
    return;
  }

  throw test_support::failure(
      "candidate from different projected control was accepted");
}

void reject_mismatched_release_binding()
{
  candidate_projection baseline =
      project_candidate(source_fixture::make_snapshot());

  source_fixture::options options;
  options.version = "2.0";
  pkgsource::source_snapshot changed =
      source_fixture::make_snapshot(std::move(options));

  try {
    candidate_projection forged(std::move(changed), baseline.candidate());
    static_cast<void>(forged);
  } catch (const projection_error& error) {
    require(error.code() == projection_error_code::source_binding,
            "release mismatch must report source binding failure");
    return;
  }

  throw test_support::failure(
      "candidate from different package release was accepted");
}

void accept_equivalent_projection_from_distinct_source_authority()
{
  candidate_projection baseline =
      project_candidate(source_fixture::make_snapshot());

  source_fixture::options options;
  options.summary = "Different excluded metadata";
  pkgsource::source_snapshot changed =
      source_fixture::make_snapshot(std::move(options));
  require(changed.identity() != baseline.source_identity(),
          "fixture must change retained source identity");

  candidate_projection rebound(std::move(changed), baseline.candidate());
  require_equal(rebound.candidate(),
                baseline.candidate(),
                "equivalent planner projection must remain admissible");
  require(rebound.source_identity() != baseline.source_identity(),
          "constructor must retain the supplied complete source authority");
}

} // namespace

int main()
{
  return test_support::run({
      {"rejects mismatched control binding", reject_mismatched_control_binding},
      {"rejects mismatched release binding", reject_mismatched_release_binding},
      {"accepts equivalent distinct source authority",
       accept_equivalent_projection_from_distinct_source_authority},
  });
}
