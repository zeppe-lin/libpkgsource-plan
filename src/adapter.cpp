// SPDX-FileCopyrightText: 2026 Alexandr Savca
// SPDX-License-Identifier: GPL-3.0-or-later
#include <libpkgsource-plan/adapter.h>

#include "internal/candidate_control_identity.h"
#include "internal/identity_error.h"
#include "internal/source_release_identity.h"

#include <libpkgplan/control.h>
#include <libpkgplan/fact_error.h>

#include <array>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace pkgsource::plan_adapter {
namespace {

constexpr std::string_view shell_program_format = "text/x-posix-shell";
constexpr std::string_view target_architectures_fact_name =
    "pkgsource.target-architectures";
constexpr std::array removal_actions{
    lifecycle_action::pre_remove,
    lifecycle_action::post_remove,
};

[[nodiscard]] std::string encode_target_architectures(
    const std::vector<architecture_reference>& architectures)
{
  if (architectures.empty()) {
    return "*";
  }

  std::string value;
  for (const architecture_reference& architecture : architectures) {
    if (!value.empty()) {
      value += ',';
    }
    value += architecture.name();
  }
  return value;
}

[[nodiscard]] pkgplan::removal_lifecycle_phase
project_removal_phase(lifecycle_action action)
{
  switch (action) {
  case lifecycle_action::pre_remove:
    return pkgplan::removal_lifecycle_phase::pre_remove;
  case lifecycle_action::post_remove:
    return pkgplan::removal_lifecycle_phase::post_remove;
  case lifecycle_action::pre_install:
  case lifecycle_action::post_install:
    break;
  }

  // The caller iterates a fixed removal-only table. Reaching this branch means
  // the projection table and this exhaustive mapping no longer agree.
  throw projection_error(projection_error_code::planner_fact,
                         "installation lifecycle is not removal control");
}

[[nodiscard]] std::vector<pkgplan::runtime_dependency_declaration>
project_runtime_dependencies(const sealed_recipe& recipe)
{
  const std::vector<resolved_requirement> requirements =
      recipe.run_requirements();
  std::vector<pkgplan::runtime_dependency_declaration> dependencies;
  dependencies.reserve(requirements.size());

  for (const resolved_requirement& requirement : requirements) {
    dependencies.push_back(pkgplan::runtime_dependency_declaration::make(
        requirement.package().name()));
  }
  return dependencies;
}

[[nodiscard]] std::vector<pkgplan::removal_lifecycle_declaration>
project_removal_lifecycle(const sealed_recipe& recipe)
{
  std::vector<pkgplan::removal_lifecycle_declaration> lifecycle;
  lifecycle.reserve(removal_actions.size());

  // Removal phases are emitted in protocol order, independent of declaration
  // storage details in the source owner.
  for (const lifecycle_action action : removal_actions) {
    const lifecycle_program* program = recipe.lifecycle(action);
    if (program == nullptr) {
      continue;
    }

    lifecycle.push_back(pkgplan::removal_lifecycle_declaration::make(
        project_removal_phase(action),
        shell_program_format,
        program->value().material()));
  }
  return lifecycle;
}

[[nodiscard]] std::vector<pkgplan::target_profile_fact>
project_target_profile(const sealed_recipe& recipe)
{
  std::vector<pkgplan::target_profile_fact> profile;
  profile.push_back(pkgplan::target_profile_fact::make(
      target_architectures_fact_name,
      encode_target_architectures(recipe.architectures().target())));
  return profile;
}

[[nodiscard]] pkgplan::package_release
project_package_release(const package_release& source_release)
{
  return pkgplan::package_release(
      internal::import_package_release_identity(source_release.identity()),
      source_release.package().name(),
      source_release.version(),
      std::to_string(source_release.release()));
}

[[nodiscard]] pkgplan::candidate_control_projection
project_candidate_control(const sealed_recipe& recipe)
{
  // Construct through the planner owner before hashing. Its constructor owns
  // normalization and therefore defines the canonical control seen by the
  // identity protocol.
  return pkgplan::candidate_control_projection(
      project_runtime_dependencies(recipe),
      project_removal_lifecycle(recipe),
      project_target_profile(recipe));
}

} // namespace

projection_error::projection_error(projection_error_code code,
                                   std::string message)
    : std::runtime_error(std::move(message)), code_(code)
{
}

projection_error_code projection_error::code() const noexcept
{
  return code_;
}

candidate_projection::candidate_projection(
    pkgsource::source_snapshot source,
    pkgplan::candidate_package_fact candidate)
    : source_(std::move(source)), candidate_(std::move(candidate))
{
}

const pkgsource::source_snapshot& candidate_projection::source() const noexcept
{
  return source_;
}

const pkgsource::source_snapshot_identity&
candidate_projection::source_identity() const noexcept
{
  return source_.identity();
}

const pkgplan::candidate_package_fact&
candidate_projection::candidate() const noexcept
{
  return candidate_;
}

candidate_projection project_candidate(pkgsource::source_snapshot source)
{
  try {
    const sealed_recipe& recipe = source.recipe();
    pkgplan::candidate_control_projection control =
        project_candidate_control(recipe);
    pkgplan::package_release release =
        project_package_release(recipe.release());

    // Identity must be computed before control ownership is transferred. The
    // result must never depend on function-argument evaluation order or the
    // state of a moved-from control object.
    const pkgplan::candidate_control_identity identity =
        internal::compute_candidate_control_identity(control);

    pkgplan::candidate_package_fact candidate(
        identity, std::move(release), std::move(control));
    return candidate_projection(std::move(source), std::move(candidate));
  } catch (const projection_error&) {
    throw;
  } catch (const internal::identity_error& error) {
    throw projection_error(projection_error_code::identity, error.what());
  } catch (const pkgplan::fact_error& error) {
    // Translate only the planner owner's validation exception. Allocation,
    // logic, and unrelated runtime failures retain their original type.
    throw projection_error(
        projection_error_code::planner_fact,
        std::string("planner rejected source candidate projection: ") +
            error.what());
  }
}

} // namespace pkgsource::plan_adapter
