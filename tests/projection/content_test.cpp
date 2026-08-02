// SPDX-FileCopyrightText: 2026 Alexandr Savca
// SPDX-License-Identifier: GPL-3.0-or-later
#include <libpkgsource-plan/libpkgsource-plan.h>

#include "../support/source_fixture.h"
#include "../support/test_support.h"

#include <cstddef>
#include <string>
#include <utility>

namespace {

using pkgsource::plan_adapter::candidate_projection;
using pkgsource::plan_adapter::project_candidate;
using test_support::require;
using test_support::require_equal;

void projects_owned_candidate_facts()
{
  const candidate_projection projection =
      project_candidate(source_fixture::make_snapshot());
  const pkgplan::candidate_package_fact& candidate = projection.candidate();

  require_equal(projection.source_identity(),
                projection.source().identity(),
                "projection must retain the issuing source identity");
  require_equal(projection.source().origin().document(),
                std::string("recipe.yml"),
                "projection must retain source provenance");

  require_equal(candidate.release().name(),
                std::string("example"),
                "package name must cross the boundary");
  require_equal(candidate.release().version(),
                std::string("1.0"),
                "package version must cross the boundary");
  require_equal(candidate.release().release(),
                std::string("1"),
                "distribution release must cross the boundary");
  require_equal(
      candidate.release().identity().string(),
      std::string("v1:sha256:") +
          projection.source().recipe().release().identity().hex(),
      "source release identity bytes must cross without reinterpretation");

  const pkgplan::candidate_control_projection& control =
      candidate.control_projection();
  require_equal(control.runtime_dependencies().size(),
                std::size_t{1},
                "one runtime requirement must be projected");
  require_equal(control.runtime_dependencies().front().expression(),
                std::string("libfoo"),
                "runtime package name must be preserved");

  require_equal(control.removal_lifecycle().size(),
                std::size_t{2},
                "both removal phases must be projected");
  require(control.removal_lifecycle()[0].phase() ==
              pkgplan::removal_lifecycle_phase::pre_remove,
          "pre-remove must remain the first normalized phase");
  require_equal(control.removal_lifecycle()[0].format(),
                std::string("text/x-posix-shell"),
                "removal program format must be explicit");
  require_equal(control.removal_lifecycle()[0].material(),
                std::string("echo pre-remove\n"),
                "pre-remove bytes must be preserved");
  require(control.removal_lifecycle()[1].phase() ==
              pkgplan::removal_lifecycle_phase::post_remove,
          "post-remove must remain the second normalized phase");
  require_equal(control.removal_lifecycle()[1].material(),
                std::string("echo post-remove\n"),
                "post-remove bytes must be preserved");

  require_equal(control.target_profile().size(),
                std::size_t{1},
                "one target-architecture fact must be projected");
  require_equal(control.target_profile().front().name(),
                std::string("pkgsource.target-architectures"),
                "target-architecture fact name must be stable");
  require_equal(control.target_profile().front().value(),
                std::string("x86_64"),
                "target architecture must be preserved");
}

void represents_unrestricted_target_architectures()
{
  source_fixture::options options;
  options.target_architectures.clear();

  const candidate_projection projection =
      project_candidate(source_fixture::make_snapshot(std::move(options)));
  const auto& profile =
      projection.candidate().control_projection().target_profile();

  require_equal(profile.size(),
                std::size_t{1},
                "unrestricted control still needs one named fact");
  require_equal(profile.front().value(),
                std::string("*"),
                "an empty target set must map to the wildcard value");
}

void uses_source_normalized_target_order()
{
  source_fixture::options options;
  options.target_architectures = {
      pkgsource::architecture_reference("x86_64"),
      pkgsource::architecture_reference("aarch64"),
  };

  const candidate_projection projection =
      project_candidate(source_fixture::make_snapshot(std::move(options)));
  const auto& profile =
      projection.candidate().control_projection().target_profile();

  require_equal(profile.front().value(),
                std::string("aarch64,x86_64"),
                "target architectures must use source-normalized order");
}

} // namespace

int main()
{
  return test_support::run({
      {"projects owner facts", projects_owned_candidate_facts},
      {"represents unrestricted targets",
       represents_unrestricted_target_architectures},
      {"uses normalized target order", uses_source_normalized_target_order},
  });
}
