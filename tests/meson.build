# SPDX-FileCopyrightText: 2026 Alexandr Savca
# SPDX-License-Identifier: GPL-3.0-or-later
if get_option('tests').enabled()
  sha256_provider_test = executable(
    'sha256-provider-test',
    'mechanism/sha256_test.cpp',
    include_directories: internal_inc,
    dependencies: identity_dependencies,
    link_with: libpkgsource_plan_identity,
    install: false,
  )
  test('validate private SHA-256 provider', sha256_provider_test, suite: 'mechanism')

  candidate_control_identity_test = executable(
    'candidate-control-identity-test',
    'protocol/candidate_control_identity_test.cpp',
    include_directories: internal_inc,
    dependencies: identity_dependencies,
    link_with: libpkgsource_plan_identity,
    install: false,
  )
  test(
    'validate candidate-control identity protocol',
    candidate_control_identity_test,
    suite: 'protocol',
  )

  source_release_identity_test = executable(
    'source-release-identity-test',
    'protocol/source_release_identity_test.cpp',
    include_directories: internal_inc,
    dependencies: identity_dependencies,
    link_with: libpkgsource_plan_identity,
    install: false,
  )
  test(
    'validate source release identity import',
    source_release_identity_test,
    suite: 'protocol',
  )

  projection_content_test = executable(
    'projection-content-test',
    'unit/content_test.cpp',
    dependencies: libpkgsource_plan_dep,
    install: false,
  )
  test('validate projected candidate content', projection_content_test, suite: 'unit')

  projection_exclusions_test = executable(
    'projection-exclusions-test',
    'unit/exclusions_test.cpp',
    dependencies: libpkgsource_plan_dep,
    install: false,
  )
  test('validate excluded source authority', projection_exclusions_test, suite: 'unit')

  projection_identity_test = executable(
    'projection-identity-test',
    'integration/identity_test.cpp',
    dependencies: libpkgsource_plan_dep,
    install: false,
  )
  test(
    'compose source authority into candidate identity',
    projection_identity_test,
    suite: 'integration',
  )

  projection_binding_test = executable(
    'projection-binding-test',
    'integration/binding_test.cpp',
    dependencies: libpkgsource_plan_dep,
    install: false,
  )
  test(
    'reject mismatched source and candidate authority',
    projection_binding_test,
    suite: 'integration',
  )

  public_umbrella = executable(
    'public-umbrella',
    'header/umbrella_header_test.cpp',
    dependencies: libpkgsource_plan_dep,
    install: false,
  )
  test('compile public umbrella header', public_umbrella, suite: 'header')

  public_component = executable(
    'public-component',
    'header/adapter_header_test.cpp',
    dependencies: libpkgsource_plan_dep,
    install: false,
  )
  test('compile adapter header independently', public_component, suite: 'header')

  sh = find_program('sh')
  if library_kind == 'shared'
    nm = find_program('nm')
    test(
      'validate exact shared-library ABI surface',
      sh,
      args: [
        join_paths(meson.current_source_dir(), 'contracts/check_abi_surface.sh'),
        nm.full_path(),
        libpkgsource_plan.full_path(),
        meson.project_source_root() / 'abi/libpkgsource-plan.exports',
      ],
      suite: 'contracts',
    )
  endif

  foreach contract : [
    ['owner-boundary', 'check_owner_boundary.sh', [meson.project_source_root()]],
    ['release-contract', 'check_release_contract.sh', [meson.project_source_root()]],
    ['test-layout', 'check_test_layout.sh', [meson.project_source_root()]],
    ['metadata', 'metadata_test.sh', [meson.project_build_root(), meson.project_version()]],
    ['documentation-contract', 'check_documentation_contract.sh', [meson.project_source_root()]],
    ['documentation-install-contract', 'check_documentation_install_contract.sh', [meson.project_source_root()]],
    ['html-docs-contract', 'check_html_docs_contract.sh', [meson.project_source_root()]],
    ['style-contract', 'check_style_contract.sh', [meson.project_source_root(), meson.project_build_root()]],
    ['manpage-source', 'check_manpage_source.sh', [meson.project_source_root()]],
    ['manpage-normalizer', 'check_manpage_normalizer.sh', [meson.project_source_root()]],
  ]
    test(
      contract[0],
      sh,
      args: [join_paths(meson.current_source_dir(), 'contracts', contract[1])] + contract[2],
      suite: 'contracts',
    )
  endforeach

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
      suite: 'contracts',
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
      suite: 'contracts',
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
      suite: 'contracts',
    )
  endif
endif
