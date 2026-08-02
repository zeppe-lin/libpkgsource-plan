// SPDX-FileCopyrightText: 2026 Alexandr Savca
// SPDX-License-Identifier: GPL-3.0-or-later
#include "../../src/internal/sha256.h"
#include "../support/test_support.h"

#include <cstddef>
#include <cstdint>
#include <string>
#include <string_view>

namespace {

using pkgsource::plan_adapter::internal::sha256_context;
using pkgsource::plan_adapter::internal::sha256_digest;
using test_support::require_equal;

[[nodiscard]] std::string hexadecimal(const sha256_digest& digest)
{
  constexpr std::string_view digits = "0123456789abcdef";
  std::string result;
  result.reserve(digest.size() * 2);
  for (const std::uint8_t byte : digest) {
    result += digits[byte >> 4];
    result += digits[byte & 0x0f];
  }
  return result;
}

[[nodiscard]] sha256_digest digest(std::string_view material)
{
  sha256_context context;
  context.update(material);
  return context.finish();
}

void hashes_empty_input()
{
  require_equal(
      hexadecimal(digest("")),
      std::string("e3b0c44298fc1c149afbf4c8996fb924"
                  "27ae41e4649b934ca495991b7852b855"),
      "selected provider must implement the SHA-256 empty-input vector");
}

void hashes_incremental_input()
{
  sha256_context context;
  context.update("a");
  context.update("b");
  context.update("c");

  require_equal(hexadecimal(context.finish()),
                std::string("ba7816bf8f01cfea414140de5dae2223"
                            "b00361a396177a9cb410ff61f20015ad"),
                "provider chunking must not change SHA-256 material");
}

void hashes_binary_input()
{
  const std::uint8_t material[]{0x00, 0x01, 0x00, 0xff};
  sha256_context context;
  context.update(material, sizeof(material));

  require_equal(hexadecimal(context.finish()),
                std::string("35405bce5dc7cacedd9c4373e68d01b36"
                            "9da4b5da34ecf90d0db265416797f71"),
                "provider must hash exact binary bytes");
}

} // namespace

int main()
{
  return test_support::run({
      {"hashes empty input", hashes_empty_input},
      {"hashes incremental input", hashes_incremental_input},
      {"hashes binary input", hashes_binary_input},
  });
}
