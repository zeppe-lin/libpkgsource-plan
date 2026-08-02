// SPDX-FileCopyrightText: 2026 Alexandr Savca
// SPDX-License-Identifier: GPL-3.0-or-later
#include "candidate_control_identity.h"

#include "identity_error.h"
#include "sha256.h"

#include <array>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <string>
#include <string_view>

namespace pkgsource::plan_adapter::internal {
namespace {

constexpr std::string_view identity_domain =
    "libpkgsource-plan/candidate-control/v1";

class canonical_identity_writer final {
public:
  void write_u64(std::uint64_t value)
  {
    std::array<std::uint8_t, 8> bytes{};
    for (std::size_t index = 0; index < bytes.size(); ++index) {
      bytes[bytes.size() - 1 - index] =
          static_cast<std::uint8_t>(value >> (index * 8));
    }
    context_.update(bytes.data(), bytes.size());
  }

  void write_sequence_size(std::size_t value)
  {
    if constexpr (sizeof(std::size_t) > sizeof(std::uint64_t)) {
      if (value > std::numeric_limits<std::uint64_t>::max()) {
        throw identity_error("identity sequence exceeds canonical u64 length");
      }
    }
    write_u64(static_cast<std::uint64_t>(value));
  }

  void write_text(std::string_view value)
  {
    write_sequence_size(value.size());
    context_.update(value);
  }

  [[nodiscard]] sha256_digest finish()
  {
    return context_.finish();
  }

private:
  sha256_context context_;
};

[[nodiscard]] std::uint64_t
identity_phase_code(pkgplan::removal_lifecycle_phase phase)
{
  switch (phase) {
  case pkgplan::removal_lifecycle_phase::pre_remove:
    return 1;
  case pkgplan::removal_lifecycle_phase::post_remove:
    return 2;
  }
  throw identity_error("unknown removal lifecycle phase");
}

} // namespace

pkgplan::candidate_control_identity compute_candidate_control_identity(
    const pkgplan::candidate_control_projection& control)
{
  try {
    canonical_identity_writer writer;

    // Field order and framing are the published candidate-control/v1 protocol.
    // Do not refactor this sequence independently of the normative document and
    // its fixed vectors.
    writer.write_text(identity_domain);

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
    for (const pkgplan::target_profile_fact& profile :
         control.target_profile()) {
      writer.write_text(profile.name());
      writer.write_text(profile.value());
    }

    return pkgplan::candidate_control_identity::from_sha256(writer.finish());
  } catch (const sha256_error& error) {
    throw identity_error(std::string("candidate-control SHA-256 failed: ") +
                         error.what());
  }
}

} // namespace pkgsource::plan_adapter::internal
