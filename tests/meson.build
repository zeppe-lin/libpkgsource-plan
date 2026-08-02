# SPDX-FileCopyrightText: 2026 Alexandr Savca
# SPDX-License-Identifier: GPL-3.0-or-later
if get_option('tests').enabled()
  adapter_test = executable(
    'adapter-test',
    'plan_adapter_test.cpp',
    dependencies: libpkgsource_plan_dep,
    install: false,
  )
  test('adapter', adapter_test)

  public_headers = executable(
    'public-headers',
    'plan_adapter_header_test.cpp',
    dependencies: libpkgsource_plan_dep,
    install: false,
  )
  test('public-headers', public_headers)
endif
