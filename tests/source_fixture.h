// SPDX-FileCopyrightText: 2026 Alexandr Savca
// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include <libpkgsource/libpkgsource.h>

#include <cstdint>
#include <optional>
#include <string>
#include <utility>
#include <vector>

namespace source_fixture {

struct options final {
  std::string origin_document = "recipe.yml";
  std::string version = "1.0";
  std::uint32_t release = 1;
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
  std::vector<pkgsource::architecture_reference> build_architectures{
      pkgsource::architecture_reference("aarch64"),
  };
  std::vector<pkgsource::architecture_reference> target_architectures{
      pkgsource::architecture_reference("x86_64"),
  };
};

inline pkgsource::declaration_provenance provenance(const char* path,
                                                    std::uint32_t line)
{
  return pkgsource::declaration_provenance("recipe.yml", path, line, 3);
}

inline pkgsource::profile_catalog
make_profile_catalog(std::string profile_member)
{
  return pkgsource::profile_catalog::seal({
      pkgsource::profile_declaration(
          pkgsource::profile_reference("@toolchain"),
          pkgsource::declaration_provenance("profiles.yml", "toolchain", 1, 1),
          {pkgsource::profile_member_declaration(
              pkgsource::requirement_subject(
                  pkgsource::package_reference(std::move(profile_member))),
              pkgsource::declaration_provenance(
                  "profiles.yml", "toolchain.members[0]", 2, 3))}),
  });
}

inline pkgsource::source_snapshot make_snapshot(options values = {})
{
  const pkgsource::profile_catalog profiles =
      make_profile_catalog(std::move(values.build_profile_member));

  std::vector<pkgsource::source_input> sources;
  sources.push_back(pkgsource::source_input::remote(
      std::move(values.source_url),
      "example.tar.gz",
      pkgsource::digest(pkgsource::digest_algorithm::sha256,
                        "0123456789abcdef0123456789abcdef"
                        "0123456789abcdef0123456789abcdef")));

  return pkgsource::seal_source(
      pkgsource::source_origin(std::move(values.origin_document)),
      pkgsource::recipe_declaration(
          pkgsource::package_release(pkgsource::package_reference("example"),
                                     std::move(values.version),
                                     values.release),
          pkgsource::package_metadata(
              std::move(values.summary), std::nullopt, std::nullopt, {"MIT"}),
          std::move(sources),
          pkgsource::program(pkgsource::program_language::posix_shell,
                             std::move(values.build_script)),
          {
              pkgsource::requirement_declaration(
                  pkgsource::requirement_scope::build(),
                  pkgsource::requirement_subject(
                      pkgsource::profile_reference("@toolchain")),
                  provenance("requirements.build[0]", 10)),
              pkgsource::requirement_declaration(
                  pkgsource::requirement_scope::run(),
                  pkgsource::requirement_subject(pkgsource::package_reference(
                      std::move(values.run_dependency))),
                  provenance("requirements.run[0]", 12)),
              pkgsource::requirement_declaration(
                  pkgsource::requirement_scope::check(),
                  pkgsource::requirement_subject(pkgsource::package_reference(
                      std::move(values.check_dependency))),
                  provenance("requirements.check[0]", 14)),
              pkgsource::requirement_declaration(
                  pkgsource::requirement_scope::lifecycle(
                      pkgsource::lifecycle_action::pre_remove),
                  pkgsource::requirement_subject(pkgsource::package_reference(
                      std::move(values.lifecycle_dependency))),
                  provenance("requirements.lifecycle.pre-remove[0]", 16)),
          },
          {
              pkgsource::lifecycle_program(
                  pkgsource::lifecycle_action::pre_install,
                  pkgsource::program(pkgsource::program_language::posix_shell,
                                     std::move(values.pre_install_script))),
              pkgsource::lifecycle_program(
                  pkgsource::lifecycle_action::post_install,
                  pkgsource::program(pkgsource::program_language::posix_shell,
                                     std::move(values.post_install_script))),
              pkgsource::lifecycle_program(
                  pkgsource::lifecycle_action::pre_remove,
                  pkgsource::program(pkgsource::program_language::posix_shell,
                                     std::move(values.pre_remove_script))),
              pkgsource::lifecycle_program(
                  pkgsource::lifecycle_action::post_remove,
                  pkgsource::program(pkgsource::program_language::posix_shell,
                                     std::move(values.post_remove_script))),
          },
          pkgsource::architecture_requirements(
              std::move(values.build_architectures),
              std::move(values.target_architectures)),
          provenance("$", 1),
          pkgsource::program(pkgsource::program_language::posix_shell,
                             std::move(values.check_script))),
      profiles);
}

} // namespace source_fixture
