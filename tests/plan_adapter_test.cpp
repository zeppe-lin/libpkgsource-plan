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
  std::string summary = "Example";
  std::string source_url = "https://example.invalid/example-1.0.tar.gz";
  std::string run_dependency = "libfoo";
  std::string build_profile_member = "binutils";
  std::string check_dependency = "pkgcheck";
  std::string lifecycle_dependency = "remove-helper";
  std::string build_script = "echo build\n";
  std::string check_script = "echo check\n";
  std::string pre_remove_script = "echo pre-remove\n";
  std::string post_remove_script = "echo post-remove\n";
  std::string pre_install_script = "echo pre-install\n";
  std::string post_install_script = "echo post-install\n";
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
              requirement_subject(
                  package_reference(std::move(options.build_profile_member))),
              declaration_provenance(
                  "profiles.yml", "toolchain.members[0]", 2, 3))}),
  });

  std::vector<source_input> sources;
  sources.push_back(source_input::remote(
      std::move(options.source_url), "example.tar.gz",
      digest(digest_algorithm::sha256,
             "0123456789abcdef0123456789abcdef"
             "0123456789abcdef0123456789abcdef")));

  return seal_source(
      source_origin("recipe.yml"),
      recipe_declaration(
          package_release(
              package_reference("example"), std::move(options.version), 1),
          package_metadata(std::move(options.summary), std::nullopt,
                           std::nullopt, {"MIT"}),
          std::move(sources),
          program(program_language::posix_shell,
                  std::move(options.build_script)),
          {
            requirement_declaration(
                requirement_scope::build(),
                requirement_subject(profile_reference("@toolchain")),
                at("requirements.build[0]", 10)),
            requirement_declaration(
                requirement_scope::run(),
                requirement_subject(
                    package_reference(std::move(options.run_dependency))),
                at("requirements.run[0]", 12)),
            requirement_declaration(
                requirement_scope::check(),
                requirement_subject(
                    package_reference(std::move(options.check_dependency))),
                at("requirements.check[0]", 14)),
            requirement_declaration(
                requirement_scope::lifecycle(lifecycle_action::pre_remove),
                requirement_subject(
                    package_reference(std::move(options.lifecycle_dependency))),
                at("requirements.lifecycle.pre-remove[0]", 16)),
          },
          {
            lifecycle_program(
                lifecycle_action::pre_install,
                program(program_language::posix_shell,
                        std::move(options.pre_install_script))),
            lifecycle_program(
                lifecycle_action::post_install,
                program(program_language::posix_shell,
                        std::move(options.post_install_script))),
            lifecycle_program(
                lifecycle_action::pre_remove,
                program(program_language::posix_shell,
                        std::move(options.pre_remove_script))),
            lifecycle_program(
                lifecycle_action::post_remove,
                program(program_language::posix_shell,
                        std::move(options.post_remove_script))),
          },
          architecture_requirements(
              std::move(options.build_architectures),
              std::move(options.target_architectures)),
          at("$", 1),
          program(program_language::posix_shell,
                  std::move(options.check_script))),
      catalog);
}

void test_projection_content_and_fixed_identity()
{
  const plan_adapter::candidate_projection projected =
      plan_adapter::project_candidate(snapshot());
  const auto& candidate = projected.candidate();

  assert(projected.source_identity() == projected.source().identity());
  assert(projected.source().origin().document() == "recipe.yml");
  assert(candidate.release().name() == "example");
  assert(candidate.release().version() == "1.0");
  assert(candidate.release().release() == "1");
  assert(candidate.release().identity().string()
         == "v1:sha256:"
            + projected.source().recipe().release().identity().hex());

  const auto& control = candidate.control_projection();
  assert(control.runtime_dependencies().size() == 1);
  assert(control.runtime_dependencies()[0].expression() == "libfoo");

  assert(control.removal_lifecycle().size() == 2);
  assert(control.removal_lifecycle()[0].phase()
         == pkgplan::removal_lifecycle_phase::pre_remove);
  assert(control.removal_lifecycle()[0].format() == "text/x-posix-shell");
  assert(control.removal_lifecycle()[0].material() == "echo pre-remove\n");
  assert(control.removal_lifecycle()[1].phase()
         == pkgplan::removal_lifecycle_phase::post_remove);
  assert(control.removal_lifecycle()[1].material() == "echo post-remove\n");

  assert(control.target_profile().size() == 1);
  assert(control.target_profile()[0].name()
         == "pkgsource.target-architectures");
  assert(control.target_profile()[0].value() == "x86_64");

  assert(candidate.identity().string()
         == "v1:sha256:2064db1e0c8a2934b1998aae9cd289cf"
            "4faace757dbc0f153afd017a85781795");
}

void test_excluded_source_facts_do_not_change_candidate()
{
  const auto original = plan_adapter::project_candidate(snapshot());

  source_options build_and_check;
  build_and_check.build_profile_member = "clang";
  build_and_check.check_dependency = "pkgtest";
  build_and_check.build_script = "ninja -C build\n";
  build_and_check.check_script = "meson test -C build\n";
  build_and_check.build_architectures = {
      architecture_reference("x86_64")};
  const auto changed_build_and_check = plan_adapter::project_candidate(
      snapshot(std::move(build_and_check)));
  assert(original.source_identity()
         != changed_build_and_check.source_identity());
  assert(original.candidate() == changed_build_and_check.candidate());

  source_options material;
  material.summary = "Changed summary";
  material.source_url = "https://mirror.invalid/example-1.0.tar.gz";
  material.pre_install_script = "echo changed pre-install\n";
  material.post_install_script = "echo changed post-install\n";
  const auto changed_material = plan_adapter::project_candidate(
      snapshot(std::move(material)));
  assert(original.source_identity() != changed_material.source_identity());
  assert(original.candidate() == changed_material.candidate());

  source_options lifecycle_requirement;
  lifecycle_requirement.lifecycle_dependency = "other-remove-helper";
  const auto changed_lifecycle_requirement = plan_adapter::project_candidate(
      snapshot(std::move(lifecycle_requirement)));
  assert(original.source_identity()
         != changed_lifecycle_requirement.source_identity());
  assert(original.candidate() == changed_lifecycle_requirement.candidate());
}

void test_projected_control_changes_control_identity()
{
  const auto original = plan_adapter::project_candidate(snapshot());

  source_options runtime_options;
  runtime_options.run_dependency = "libbar";
  const auto runtime = plan_adapter::project_candidate(
      snapshot(std::move(runtime_options)));
  assert(original.candidate().identity() != runtime.candidate().identity());
  assert(original.candidate().release() == runtime.candidate().release());

  source_options pre_remove_options;
  pre_remove_options.pre_remove_script = "echo changed pre-remove\n";
  const auto pre_remove = plan_adapter::project_candidate(
      snapshot(std::move(pre_remove_options)));
  assert(original.candidate().identity()
         != pre_remove.candidate().identity());

  source_options post_remove_options;
  post_remove_options.post_remove_script = "echo changed post-remove\n";
  const auto post_remove = plan_adapter::project_candidate(
      snapshot(std::move(post_remove_options)));
  assert(original.candidate().identity()
         != post_remove.candidate().identity());

  source_options architecture_options;
  architecture_options.target_architectures = {
      architecture_reference("aarch64"),
      architecture_reference("x86_64")};
  const auto architecture = plan_adapter::project_candidate(
      snapshot(std::move(architecture_options)));
  assert(original.candidate().identity()
         != architecture.candidate().identity());
}

void test_release_changes_fact_not_control_identity()
{
  const auto original = plan_adapter::project_candidate(snapshot());

  source_options release_options;
  release_options.version = "2.0";
  const auto changed = plan_adapter::project_candidate(
      snapshot(std::move(release_options)));

  assert(original.candidate().identity() == changed.candidate().identity());
  assert(original.candidate().control_projection()
         == changed.candidate().control_projection());
  assert(original.candidate().release().identity()
         != changed.candidate().release().identity());
  assert(original.candidate() != changed.candidate());
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
  test_projection_content_and_fixed_identity();
  test_excluded_source_facts_do_not_change_candidate();
  test_projected_control_changes_control_identity();
  test_release_changes_fact_not_control_identity();
  test_unrestricted_target_architecture();
}
