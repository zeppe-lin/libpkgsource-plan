# SPDX-FileCopyrightText: 2026 Alexandr Savca
# SPDX-License-Identifier: GPL-3.0-or-later
if get_option('tests').enabled()
  sha256_provider_test = executable(
    'sha256-provider-test',
    'internal/sha256_test.cpp',
    include_directories: internal_inc,
    dependencies: identity_dependencies,
    link_with: libpkgsource_plan_identity,
    install: false,
  )
  test('sha256-provider', sha256_provider_test)

  candidate_control_identity_test = executable(
    'candidate-control-identity-test',
    'internal/candidate_control_identity_test.cpp',
    include_directories: internal_inc,
    dependencies: identity_dependencies,
    link_with: libpkgsource_plan_identity,
    install: false,
  )
  test('candidate-control-identity', candidate_control_identity_test)

  source_release_identity_test = executable(
    'source-release-identity-test',
    'internal/source_release_identity_test.cpp',
    include_directories: internal_inc,
    dependencies: identity_dependencies,
    link_with: libpkgsource_plan_identity,
    install: false,
  )
  test('source-release-identity', source_release_identity_test)

  projection_content_test = executable(
    'projection-content-test',
    'projection/content_test.cpp',
    dependencies: libpkgsource_plan_dep,
    install: false,
  )
  test('projection-content', projection_content_test)

  projection_identity_test = executable(
    'projection-identity-test',
    'projection/identity_test.cpp',
    dependencies: libpkgsource_plan_dep,
    install: false,
  )
  test('projection-identity', projection_identity_test)

  projection_exclusions_test = executable(
    'projection-exclusions-test',
    'projection/exclusions_test.cpp',
    dependencies: libpkgsource_plan_dep,
    install: false,
  )
  test('projection-exclusions', projection_exclusions_test)

  public_umbrella = executable(
    'public-umbrella',
    'public/umbrella_header_test.cpp',
    dependencies: libpkgsource_plan_dep,
    install: false,
  )
  test('public-umbrella', public_umbrella)

  public_component = executable(
    'public-component',
    'public/adapter_header_test.cpp',
    dependencies: libpkgsource_plan_dep,
    install: false,
  )
  test('public-component', public_component)

  sh = find_program('sh')
  if library_kind == 'shared'
    nm = find_program('nm')
    test(
      'abi-surface',
      sh,
      args: [
        join_paths(meson.current_source_dir(), 'contracts/check_abi_surface.sh'),
        nm.full_path(),
        libpkgsource_plan.full_path(),
        meson.project_source_root() / 'abi/libpkgsource-plan.exports',
      ],
    )
  endif

  test(
    'owner-boundary',
    sh,
    args: [
      join_paths(meson.current_source_dir(), 'contracts/check_owner_boundary.sh'),
      meson.project_source_root(),
    ],
  )
  test(
    'release-contract',
    sh,
    args: [
      join_paths(meson.current_source_dir(), 'contracts/check_release_contract.sh'),
      meson.project_source_root(),
    ],
  )
  test(
    'metadata',
    sh,
    args: [
      join_paths(meson.current_source_dir(), 'contracts/metadata_test.sh'),
      meson.project_build_root(),
      meson.project_version(),
    ],
  )
  test(
    'documentation-contract',
    sh,
    args: [
      join_paths(meson.current_source_dir(), 'contracts/check_documentation_contract.sh'),
      meson.project_source_root(),
    ],
  )
  test(
    'documentation-install-contract',
    sh,
    args: [
      join_paths(meson.current_source_dir(), 'contracts/check_documentation_install_contract.sh'),
      meson.project_source_root(),
    ],
  )
  test(
    'html-docs-contract',
    sh,
    args: [
      join_paths(meson.current_source_dir(), 'contracts/check_html_docs_contract.sh'),
      meson.project_source_root(),
    ],
  )
  test(
    'style-contract',
    sh,
    args: [
      join_paths(meson.current_source_dir(), 'contracts/check_style_contract.sh'),
      meson.project_source_root(),
      meson.project_build_root(),
    ],
  )
  test(
    'manpage-source',
    sh,
    args: [
      join_paths(meson.current_source_dir(), 'contracts/check_manpage_source.sh'),
      meson.project_source_root(),
    ],
  )

  test(
    'manpage-normalizer',
    sh,
    args: [
      join_paths(meson.current_source_dir(), 'contracts/check_manpage_normalizer.sh'),
      meson.project_source_root(),
    ],
  )

  pandoc = find_program('pandoc', required: false)
  if pandoc.found()
    test(
      'manpage-generated',
      sh,
      args: [
        join_paths(meson.current_source_dir(), 'contracts/check_manpage_generated.sh'),
        meson.project_source_root(),
        pandoc.full_path(),
      ],
    )
  endif

  clang_format = find_program('clang-format-17', required: false)
  if clang_format.found()
    test(
      'format',
      sh,
      args: [
        join_paths(meson.current_source_dir(), 'contracts/check_format.sh'),
        clang_format.full_path(),
        meson.project_source_root(),
      ],
    )
  endif

  if html_docs_enabled
    html_docs_checker = find_program(
      meson.project_source_root() / 'tools/check-html-docs.py',
    )
    test(
      'html-docs',
      html_docs_checker,
      args: [
        html_docs_output_dir,
        '--forbid-path', meson.project_source_root(),
        '--forbid-path', meson.project_build_root(),
      ],
      depends: [html_docs_target],
    )
  endif
endif
