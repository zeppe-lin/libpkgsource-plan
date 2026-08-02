// SPDX-FileCopyrightText: 2026 Alexandr Savca
// SPDX-License-Identifier: GPL-3.0-or-later
#include <libpkgsource-plan/adapter.h>

#include <libpkgplan/control.h>
#include <libpkgplan/digest.h>
#include <libpkgplan/fact_error.h>

#include <openssl/evp.h>

#include <array>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <memory>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace pkgsource::plan_adapter {
namespace {

constexpr std::string_view candidate_control_identity_domain =
    "libpkgsource-plan/candidate-control/v1";
constexpr std::string_view shell_program_format = "text/x-posix-shell";
constexpr std::string_view target_architectures_fact_name =
    "pkgsource.target-architectures";
constexpr std::array removal_actions{
    lifecycle_action::pre_remove,
    lifecycle_action::post_remove,
};

using digest_context = std::unique_ptr<EVP_MD_CTX, decltype(&EVP_MD_CTX_free)>;

class candidate_control_identity_writer final {
public:
  candidate_control_identity_writer()
      : context_(EVP_MD_CTX_new(), EVP_MD_CTX_free)
  {
    if (!context_ ||
        EVP_DigestInit_ex(context_.get(), EVP_sha256(), nullptr) != 1) {
      throw projection_error(projection_error_code::identity,
                             "SHA-256 initialization failed");
    }
  }

  void write_u64(std::uint64_t value)
  {
    std::array<std::uint8_t, 8> bytes{};
    for (std::size_t index = 0; index < bytes.size(); ++index) {
      bytes[bytes.size() - 1 - index] =
          static_cast<std::uint8_t>(value >> (index * 8));
    }
    write_bytes(bytes.data(), bytes.size());
  }

  void write_sequence_size(std::size_t value)
  {
    if constexpr (sizeof(std::size_t) > sizeof(std::uint64_t)) {
      if (value > std::numeric_limits<std::uint64_t>::max()) {
        throw projection_error(
            projection_error_code::identity,
            "identity sequence exceeds canonical u64 length");
      }
    }
    write_u64(static_cast<std::uint64_t>(value));
  }

  void write_text(std::string_view value)
  {
    write_sequence_size(value.size());
    write_bytes(value.data(), value.size());
  }

  [[nodiscard]] pkgplan::sha256_digest_bytes finish()
  {
    pkgplan::sha256_digest_bytes bytes{};
    unsigned int size = 0;
    if (EVP_DigestFinal_ex(context_.get(), bytes.data(), &size) != 1 ||
        size != bytes.size()) {
      throw projection_error(projection_error_code::identity,
                             "SHA-256 finalization failed");
    }
    return bytes;
  }

private:
  void write_bytes(const void* data, std::size_t size)
  {
    if (size == 0) {
      return;
    }
    if (EVP_DigestUpdate(context_.get(), data, size) != 1) {
      throw projection_error(projection_error_code::identity,
                             "SHA-256 update failed");
    }
  }

  digest_context context_;
};

[[nodiscard]] std::uint8_t decode_hex_nibble(char value)
{
  if (value >= '0' && value <= '9') {
    return static_cast<std::uint8_t>(value - '0');
  }
  if (value >= 'a' && value <= 'f') {
    return static_cast<std::uint8_t>(value - 'a' + 10);
  }
  throw projection_error(projection_error_code::identity,
                         "source identity is not lowercase hexadecimal");
}

[[nodiscard]] pkgplan::sha256_digest_bytes
decode_source_release_identity(std::string_view hexadecimal)
{
  pkgplan::sha256_digest_bytes bytes{};
  if (hexadecimal.size() != bytes.size() * 2) {
    throw projection_error(projection_error_code::identity,
                           "source identity has invalid width");
  }

  for (std::size_t index = 0; index < bytes.size(); ++index) {
    const std::uint8_t high = decode_hex_nibble(hexadecimal[index * 2]);
    const std::uint8_t low = decode_hex_nibble(hexadecimal[index * 2 + 1]);
    bytes[index] = static_cast<std::uint8_t>((high << 4) | low);
  }
  return bytes;
}

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
to_planner_phase(lifecycle_action action)
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
  throw projection_error(projection_error_code::planner_fact,
                         "installation lifecycle is not removal control");
}

[[nodiscard]] std::uint64_t
identity_phase_code(pkgplan::removal_lifecycle_phase phase)
{
  switch (phase) {
  case pkgplan::removal_lifecycle_phase::pre_remove:
    return 1;
  case pkgplan::removal_lifecycle_phase::post_remove:
    return 2;
  }
  throw projection_error(projection_error_code::identity,
                         "unknown removal lifecycle phase");
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

  for (const lifecycle_action action : removal_actions) {
    const lifecycle_program* program = recipe.lifecycle(action);
    if (program == nullptr) {
      continue;
    }
    lifecycle.push_back(pkgplan::removal_lifecycle_declaration::make(
        to_planner_phase(action),
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
      pkgplan::package_release_identity::from_sha256(
          decode_source_release_identity(source_release.identity().hex())),
      source_release.package().name(),
      source_release.version(),
      std::to_string(source_release.release()));
}

[[nodiscard]] pkgplan::candidate_control_identity
compute_candidate_control_identity(
    const pkgplan::candidate_control_projection& control)
{
  candidate_control_identity_writer writer;
  writer.write_text(candidate_control_identity_domain);

  writer.write_sequence_size(control.runtime_dependencies().size());
  for (const pkgplan::runtime_dependency_declaration& dependency :
       control.runtime_dependencies()) {
    writer.write_text(dependency.expression());
  }

  writer.write_sequence_size(control.removal_lifecycle().size());
  for (const pkgplan::removal_lifecycle_declaration& lifecycle :
       control.removal_lifecycle()) {
    writer.write_u64(identity_phase_code(lifecycle.phase()));
    writer.write_text(lifecycle.format());
    writer.write_text(lifecycle.material());
  }

  writer.write_sequence_size(control.target_profile().size());
  for (const pkgplan::target_profile_fact& profile : control.target_profile()) {
    writer.write_text(profile.name());
    writer.write_text(profile.value());
  }

  return pkgplan::candidate_control_identity::from_sha256(writer.finish());
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
    pkgplan::candidate_control_projection control(
        project_runtime_dependencies(recipe),
        project_removal_lifecycle(recipe),
        project_target_profile(recipe));
    pkgplan::package_release release =
        project_package_release(recipe.release());

    const pkgplan::candidate_control_identity identity =
        compute_candidate_control_identity(control);
    pkgplan::candidate_package_fact candidate(
        identity, std::move(release), std::move(control));
    return candidate_projection(std::move(source), std::move(candidate));
  } catch (const projection_error&) {
    throw;
  } catch (const pkgplan::fact_error& error) {
    throw projection_error(
        projection_error_code::planner_fact,
        std::string("planner rejected source candidate projection: ") +
            error.what());
  }
}

} // namespace pkgsource::plan_adapter
