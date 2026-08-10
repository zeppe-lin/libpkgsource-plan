// SPDX-FileCopyrightText: 2026 Alexandr Savca
// SPDX-License-Identifier: GPL-3.0-or-later
#include <libpkgsource-plan/libpkgsource-plan.h>

#include "../support/source_fixture.h"

#include <string>

int main()
{
  const auto projection =
      pkgsource::plan_adapter::project_candidate(source_fixture::make_snapshot());
  return projection.candidate().release().name() == std::string("example")
             ? 0
             : 1;
}
