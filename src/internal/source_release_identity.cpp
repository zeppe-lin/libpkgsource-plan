// SPDX-FileCopyrightText: 2026 Alexandr Savca
// SPDX-License-Identifier: GPL-3.0-or-later
#include "source_release_identity.h"

#include "identity_error.h"

#include <cstddef>
#include <cstdint>
#include <string_view>

namespace pkgsource::plan_adapter::internal {
namespace {

[[nodiscard]] std::uint8_t decode_hex_nibble(char value)
{
  if (value >= '0' && value <= '9') {
    return static_cast<std::uint8_t>(value - '0');
  }
  if (value >= 'a' && value <= 'f') {
    return static_cast<std::uint8_t>(value - 'a' + 10);
  }
  throw identity_error("source identity is not lowercase hexadecimal");
}

} // namespace

pkgplan::package_release_identity import_package_release_identity(
    const pkgsource::package_release_identity& identity)
{
  const std::string_view hexadecimal = identity.hex();
  pkgplan::sha256_digest_bytes bytes{};
  if (hexadecimal.size() != bytes.size() * 2) {
    throw identity_error("source identity has invalid width");
  }

  for (std::size_t index = 0; index < bytes.size(); ++index) {
    const std::uint8_t high = decode_hex_nibble(hexadecimal[index * 2]);
    const std::uint8_t low = decode_hex_nibble(hexadecimal[index * 2 + 1]);
    bytes[index] = static_cast<std::uint8_t>((high << 4) | low);
  }

  return pkgplan::package_release_identity::from_sha256(bytes);
}

} // namespace pkgsource::plan_adapter::internal
