// SPDX-FileCopyrightText: 2026 Alexandr Savca
// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <stdexcept>
#include <string>
#include <string_view>

namespace pkgsource::plan_adapter::internal {

inline constexpr std::size_t sha256_digest_size = 32;
using sha256_digest = std::array<std::uint8_t, sha256_digest_size>;

/// Failure reported by the selected SHA-256 provider.
class sha256_error final : public std::runtime_error {
public:
  explicit sha256_error(std::string message);
};

/// Provider-neutral incremental SHA-256 operation.
class sha256_context final {
public:
  sha256_context();
  ~sha256_context();

  sha256_context(const sha256_context&) = delete;
  sha256_context& operator=(const sha256_context&) = delete;
  sha256_context(sha256_context&&) = delete;
  sha256_context& operator=(sha256_context&&) = delete;

  /// Append exact bytes to the digest input.
  void update(const void* data, std::size_t size);

  /// Append exact string bytes without a terminator.
  void update(std::string_view value);

  /// Finalize the operation exactly once.
  [[nodiscard]] sha256_digest finish();

private:
  class state;
  std::unique_ptr<state> state_;
};

} // namespace pkgsource::plan_adapter::internal
