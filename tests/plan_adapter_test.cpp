// SPDX-FileCopyrightText: 2026 Alexandr Savca
// SPDX-License-Identifier: GPL-3.0-or-later
#include <libpkgsource-plan/adapter.h>

#include <cassert>
#include <cstdint>
#include <optional>
#include <string>
#include <utility>
#include <vector>

using namespace pkgsource;

namespace {

declaration_provenance at(const char* path, std::uint32_t line)
{
  return declaration_provenance("recipe.yml", path, line, 3);
}

struct source_options final {
  std::string version = "1.0";
  std::string run_dependency = "libfoo";
  std::string build_dependency = "gcc";
  std::string check_script = "echo check\n";
  std::string remove_script = "echo remove\n";
  std::string install_script = "echo install\n";
  std::vector<architecture_reference> build_architectures{
      architecture_reference("aarch64")};
  std::vector<architecture_reference> target_architectures{
      architecture_reference("x86_64")};
};

source_snapshot snapshot(source_options options = {})
{
  const profile_catalog catalog = profile_catalog::seal({
      profile_declaration(
          profile_reference("@toolchain"),
          declaration_provenance("profiles.yml", "toolchain", 1, 1),
          {profile_member_declaration(
              requirement_subject(package_reference("binutils")),
              declaration_provenance(
                  "profiles.yml", "toolchain.members[0]", 2, 3))}),
  });

  return seal_source(
      source_origin("recipe.yml"),
      recipe_declaration(
          package_release(
              package_reference("example"), std::move(options.version), 1),
          package_metadata("Example", std::nullopt, std::nullopt, {"MIT"}),
          {}, program(program_language::posix_shell, "echo build\n"),
          {
            requirement_declaration(
                requirement_scope::build(),
                requirement_subject(
                    package_reference(std::move(options.build_dependency))),
                at("requirements.build[0]", 10)),
            requirement_declaration(
                requirement_scope::run(),
                requirement_subject(
                    package_reference(std::move(options.run_dependency))),
                at("requirements.run[0]", 12)),
            requirement_declaration(
                requirement_scope::check(),
                requirement_subject(package_reference("pkgcheck")),
                at("requirements.check[0]", 14)),
            requirement_declaration(
                requirement_scope::lifecycle(lifecycle_action::pre_remove),
                requirement_subject(package_reference("remove-helper")),
                at("requirements.lifecycle.pre-remove[0]", 16)),
          },
          {
            lifecycle_program(
                lifecycle_action::pre_install,
                program(program_language::posix_shell,
                        std::move(options.install_script))),
            lifecycle_program(
                lifecycle_action::pre_remove,
                program(program_language::posix_shell,
                        std::move(options.remove_script))),
          },
          architecture_requirements(
              std::move(options.build_architectures),
              std::move(options.target_architectures)),
          at("$", 1),
          program(program_language::posix_shell,
                  std::move(options.check_script))),
      catalog);
}

void test_projection_content()
{
  plan_adapter::candidate_projection projected =
      plan_adapter::project_candidate(snapshot());
  const auto& candidate = projected.candidate();

  assert(projected.source_identity() == projected.source().identity());
  assert(candidate.release().name() == "example");
  assert(candidate.release().version() == "1.0");
  assert(candidate.release().release() == "1");

  const auto& control = candidate.control_projection();
  assert(control.runtime_dependencies().size() == 1);
  assert(control.runtime_dependencies()[0].expression() == "libfoo");
  assert(control.removal_lifecycle().size() == 1);
  assert(control.removal_lifecycle()[0].material() == "echo remove\n");
  assert(control.target_profile().size() == 1);
  assert(control.target_profile()[0].name()
         == "pkgsource.target-architectures");
  assert(control.target_profile()[0].value() == "x86_64");
}

void test_excluded_source_facts_do_not_change_candidate()
{
  const auto original = plan_adapter::project_candidate(snapshot());

  source_options changed_check;
  changed_check.check_script = "meson test -C build\n";
  const auto check = plan_adapter::project_candidate(
      snapshot(std::move(changed_check)));
  assert(original.source_identity() != check.source_identity());
  assert(original.candidate().identity() == check.candidate().identity());

  source_options changed_build;
  changed_build.build_dependency = "clang";
  changed_build.build_architectures = {architecture_reference("x86_64")};
  changed_build.install_script = "echo changed install\n";
  const auto build = plan_adapter::project_candidate(
      snapshot(std::move(changed_build)));
  assert(original.source_identity() != build.source_identity());
  assert(original.candidate().identity() == build.candidate().identity());
}

void test_projected_facts_change_candidate()
{
  const auto original = plan_adapter::project_candidate(snapshot());

  source_options runtime_options;
  runtime_options.run_dependency = "libbar";
  const auto runtime = plan_adapter::project_candidate(
      snapshot(std::move(runtime_options)));
  assert(original.candidate().identity() != runtime.candidate().identity());

  source_options removal_options;
  removal_options.remove_script = "echo changed remove\n";
  const auto removal = plan_adapter::project_candidate(
      snapshot(std::move(removal_options)));
  assert(original.candidate().identity() != removal.candidate().identity());

  source_options architecture_options;
  architecture_options.target_architectures = {
      architecture_reference("aarch64"),
      architecture_reference("x86_64")};
  const auto architecture = plan_adapter::project_candidate(
      snapshot(std::move(architecture_options)));
  assert(original.candidate().identity()
         != architecture.candidate().identity());

  source_options release_options;
  release_options.version = "2.0";
  const auto release = plan_adapter::project_candidate(
      snapshot(std::move(release_options)));
  assert(original.candidate().identity() != release.candidate().identity());
}

void test_unrestricted_target_architecture()
{
  source_options options;
  options.target_architectures.clear();
  const auto projected = plan_adapter::project_candidate(
      snapshot(std::move(options)));
  const auto& profile = projected.candidate().control_projection()
      .target_profile();
  assert(profile.size() == 1);
  assert(profile[0].value() == "*");
}

} // namespace

int main()
{
  test_projection_content();
  test_excluded_source_facts_do_not_change_candidate();
  test_projected_facts_change_candidate();
  test_unrestricted_target_architecture();
}
