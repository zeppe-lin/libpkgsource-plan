// SPDX-FileCopyrightText: 2026 Alexandr Savca
// SPDX-License-Identifier: GPL-3.0-or-later
#include <libpkgsource-plan/libpkgsource-plan.h>

#include "../support/source_fixture.h"
#include "../support/test_support.h"

#include <string>
#include <string_view>
#include <utility>

namespace {

using pkgsource::plan_adapter::candidate_projection;
using pkgsource::plan_adapter::project_candidate;
using test_support::require_equal;
using test_support::require_not_equal;

using mutation = void (*)(source_fixture::options&);

struct exclusion_case final {
  std::string_view name;
  mutation apply;
};

void change_metadata(source_fixture::options& options)
{
  options.summary = "Changed summary";
}

void change_source_input(source_fixture::options& options)
{
  options.source_url = "https://mirror.invalid/example-1.0.tar.gz";
}

void change_build_profile(source_fixture::options& options)
{
  options.build_profile_member = "clang";
}

void change_check_requirement(source_fixture::options& options)
{
  options.check_dependency = "pkgtest";
}

void change_lifecycle_requirement(source_fixture::options& options)
{
  options.lifecycle_dependency = "other-remove-helper";
}

void change_build_program(source_fixture::options& options)
{
  options.build_script = "ninja -C build\n";
}

void change_check_program(source_fixture::options& options)
{
  options.check_script = "meson test -C build\n";
}

void change_build_architecture(source_fixture::options& options)
{
  options.build_architectures = {
      pkgsource::architecture_reference("x86_64"),
  };
}

void change_pre_install(source_fixture::options& options)
{
  options.pre_install_script = "echo changed pre-install\n";
}

void change_post_install(source_fixture::options& options)
{
  options.post_install_script = "echo changed post-install\n";
}

void excluded_semantic_facts_do_not_change_candidate()
{
  const candidate_projection baseline =
      project_candidate(source_fixture::make_snapshot());
  const exclusion_case cases[] = {
      {"package metadata", change_metadata},
      {"source input", change_source_input},
      {"selected build profile", change_build_profile},
      {"check requirement", change_check_requirement},
      {"lifecycle requirement", change_lifecycle_requirement},
      {"build program", change_build_program},
      {"check program", change_check_program},
      {"build architecture", change_build_architecture},
      {"pre-install program", change_pre_install},
      {"post-install program", change_post_install},
  };

  for (const exclusion_case& current : cases) {
    source_fixture::options options;
    current.apply(options);
    const candidate_projection changed =
        project_candidate(source_fixture::make_snapshot(std::move(options)));

    require_not_equal(baseline.source_identity(),
                      changed.source_identity(),
                      std::string(current.name) +
                          " must remain source authority");
    require_equal(baseline.candidate(),
                  changed.candidate(),
                  std::string(current.name) +
                      " must not cross the planner boundary");
  }
}

void source_origin_is_retained_but_not_semantic_identity()
{
  const candidate_projection baseline =
      project_candidate(source_fixture::make_snapshot());

  source_fixture::options options;
  options.origin_document = "/collections/core/example/recipe.yml";
  const candidate_projection changed =
      project_candidate(source_fixture::make_snapshot(std::move(options)));

  require_equal(baseline.source_identity(),
                changed.source_identity(),
                "diagnostic origin must not alter source identity");
  require_equal(baseline.candidate(),
                changed.candidate(),
                "diagnostic origin must not alter planner facts");
  require_not_equal(baseline.source().origin().document(),
                    changed.source().origin().document(),
                    "projection must retain the changed diagnostic origin");
}

} // namespace

int main()
{
  return test_support::run({
      {"excludes non-planner source facts",
       excluded_semantic_facts_do_not_change_candidate},
      {"retains diagnostic origin",
       source_origin_is_retained_but_not_semantic_identity},
  });
}
