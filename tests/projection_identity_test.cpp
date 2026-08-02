// SPDX-FileCopyrightText: 2026 Alexandr Savca
// SPDX-License-Identifier: GPL-3.0-or-later
#include <libpkgsource-plan/libpkgsource-plan.h>

#include "source_fixture.h"
#include "test_support.h"

#include <string>
#include <string_view>
#include <utility>

namespace {

using pkgsource::plan_adapter::candidate_projection;
using pkgsource::plan_adapter::project_candidate;
using test_support::require_equal;
using test_support::require_not_equal;

using mutation = void (*)(source_fixture::options&);

struct mutation_case final {
  std::string_view name;
  mutation apply;
};

void change_runtime_dependency(source_fixture::options& options)
{
  options.run_dependency = "libbar";
}

void change_pre_remove(source_fixture::options& options)
{
  options.pre_remove_script = "echo changed pre-remove\n";
}

void change_post_remove(source_fixture::options& options)
{
  options.post_remove_script = "echo changed post-remove\n";
}

void change_target_architectures(source_fixture::options& options)
{
  options.target_architectures = {
      pkgsource::architecture_reference("aarch64"),
      pkgsource::architecture_reference("x86_64"),
  };
}

void binds_the_published_fixed_vector()
{
  const candidate_projection projection =
      project_candidate(source_fixture::make_snapshot());

  require_equal(projection.candidate().identity().string(),
                std::string("v1:sha256:2064db1e0c8a2934b1998aae9cd289cf"
                            "4faace757dbc0f153afd017a85781795"),
                "candidate-control identity must match the published vector");
}

void projected_control_changes_control_identity()
{
  const candidate_projection baseline =
      project_candidate(source_fixture::make_snapshot());
  const mutation_case cases[] = {
      {"runtime dependency", change_runtime_dependency},
      {"pre-remove material", change_pre_remove},
      {"post-remove material", change_post_remove},
      {"target architecture", change_target_architectures},
  };

  for (const mutation_case& current : cases) {
    source_fixture::options options;
    current.apply(options);
    const candidate_projection changed =
        project_candidate(source_fixture::make_snapshot(std::move(options)));

    require_not_equal(baseline.candidate().identity(),
                      changed.candidate().identity(),
                      std::string(current.name) +
                          " must change control identity");
    require_equal(baseline.candidate().release(),
                  changed.candidate().release(),
                  std::string(current.name) +
                      " must not change package release");
  }
}

void package_release_changes_fact_not_control_identity()
{
  const candidate_projection baseline =
      project_candidate(source_fixture::make_snapshot());

  source_fixture::options options;
  options.version = "2.0";
  const candidate_projection changed =
      project_candidate(source_fixture::make_snapshot(std::move(options)));

  require_equal(baseline.candidate().identity(),
                changed.candidate().identity(),
                "package version must not enter control identity");
  require_equal(baseline.candidate().control_projection(),
                changed.candidate().control_projection(),
                "package version must not alter normalized control");
  require_not_equal(baseline.candidate().release().identity(),
                    changed.candidate().release().identity(),
                    "package version must change release identity");
  require_not_equal(baseline.candidate(),
                    changed.candidate(),
                    "package version must change the bound candidate fact");
}

} // namespace

int main()
{
  return test_support::run({
      {"binds fixed identity vector", binds_the_published_fixed_vector},
      {"control changes identity", projected_control_changes_control_identity},
      {"release remains separate from control",
       package_release_changes_fact_not_control_identity},
  });
}
