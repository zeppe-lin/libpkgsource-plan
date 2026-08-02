// SPDX-FileCopyrightText: 2026 Alexandr Savca
// SPDX-License-Identifier: GPL-3.0-or-later
#include <libpkgsource-plan/adapter.h>

#include <type_traits>

using project_function = pkgsource::plan_adapter::candidate_projection(
    pkgsource::source_snapshot);

static_assert(std::is_base_of_v<std::runtime_error,
                                pkgsource::plan_adapter::projection_error>);
static_assert(std::is_same_v<
              decltype(&pkgsource::plan_adapter::project_candidate),
              project_function*>);

int main()
{
}
