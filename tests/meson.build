# SPDX-FileCopyrightText: 2026 Alexandr Savca
# SPDX-License-Identifier: GPL-3.0-or-later
if get_option('tests').enabled()
  projection_content_test = executable(
    'projection-content-test',
    'projection_content_test.cpp',
    dependencies: libpkgsource_plan_dep,
    install: false,
  )
  test('projection-content', projection_content_test)

  projection_identity_test = executable(
    'projection-identity-test',
    'projection_identity_test.cpp',
    dependencies: libpkgsource_plan_dep,
    install: false,
  )
  test('projection-identity', projection_identity_test)

  projection_exclusions_test = executable(
    'projection-exclusions-test',
    'projection_exclusions_test.cpp',
    dependencies: libpkgsource_plan_dep,
    install: false,
  )
  test('projection-exclusions', projection_exclusions_test)

  public_umbrella = executable(
    'public-umbrella',
    'plan_umbrella_header_test.cpp',
    dependencies: libpkgsource_plan_dep,
    install: false,
  )
  test('public-umbrella', public_umbrella)

  public_component = executable(
    'public-component',
    'plan_adapter_header_test.cpp',
    dependencies: libpkgsource_plan_dep,
    install: false,
  )
  test('public-component', public_component)

  sh = find_program('sh')
  test(
    'owner-boundary',
    sh,
    args: [
      join_paths(meson.current_source_dir(), 'check_owner_boundary.sh'),
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
  test(
    'style-contract',
    sh,
    args: [
      join_paths(meson.current_source_dir(), 'check_style_contract.sh'),
      meson.project_source_root(),
    ],
  )

  clang_format = find_program('clang-format', required: false)
  if clang_format.found()
    test(
      'format',
      sh,
      args: [
        join_paths(meson.current_source_dir(), 'check_format.sh'),
        clang_format.full_path(),
        meson.project_source_root(),
      ],
    )
  endif
endif
