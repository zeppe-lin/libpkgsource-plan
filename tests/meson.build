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

  sh = find_program('sh')
  test(
    'projection-boundary',
    sh,
    args: [
      join_paths(meson.current_source_dir(), 'check_projection_boundary.sh'),
      meson.project_source_root(),
    ],
  )
  test(
    'release-contract',
    sh,
    args: [
      join_paths(meson.current_source_dir(), 'check_release_contract.sh'),
      meson.project_source_root(),
    ],
  )
  test(
    'metadata',
    sh,
    args: [
      join_paths(meson.current_source_dir(), 'metadata_test.sh'),
      meson.project_build_root(),
      meson.project_version(),
    ],
  )
  test(
    'documentation-contract',
    sh,
    args: [
      join_paths(meson.current_source_dir(), 'check_documentation_contract.sh'),
      meson.project_source_root(),
    ],
  )
endif
