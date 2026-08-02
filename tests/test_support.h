// SPDX-FileCopyrightText: 2026 Alexandr Savca
// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include <exception>
#include <initializer_list>
#include <iostream>
#include <stdexcept>
#include <string>
#include <string_view>

namespace test_support {

class failure final : public std::runtime_error {
public:
  explicit failure(std::string message) : std::runtime_error(std::move(message))
  {
  }
};

inline void require(bool condition, std::string_view message)
{
  if (!condition) {
    throw failure(std::string(message));
  }
}

template <typename Left, typename Right>
void require_equal(const Left& left,
                   const Right& right,
                   std::string_view message)
{
  require(left == right, message);
}

template <typename Left, typename Right>
void require_not_equal(const Left& left,
                       const Right& right,
                       std::string_view message)
{
  require(left != right, message);
}

struct test_case final {
  std::string_view name;
  void (*body)();
};

inline int run(std::initializer_list<test_case> cases)
{
  for (const test_case& current : cases) {
    try {
      current.body();
    } catch (const std::exception& error) {
      std::cerr << "FAIL: " << current.name << ": " << error.what() << '\n';
      return 1;
    } catch (...) {
      std::cerr << "FAIL: " << current.name << ": non-standard exception\n";
      return 1;
    }
  }
  return 0;
}

} // namespace test_support
