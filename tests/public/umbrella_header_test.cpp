// SPDX-FileCopyrightText: 2026 Alexandr Savca
// SPDX-License-Identifier: GPL-3.0-or-later
#include <libpkgsource-plan/libpkgsource-plan.h>

#include <type_traits>

using project_function =
    pkgsource::plan_adapter::candidate_projection(pkgsource::source_snapshot);

static_assert(
    std::is_same_v<decltype(&pkgsource::plan_adapter::project_candidate),
                   project_function*>);

int main()
{
}
