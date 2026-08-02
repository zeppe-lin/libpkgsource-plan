# SPDX-FileCopyrightText: 2026 Alexandr Savca
# SPDX-License-Identifier: GPL-3.0-or-later
public_inc = include_directories('../include')
internal_inc = include_directories('.')

sha256_provider = get_option('sha256_provider')
sha256_provider_sources = []
sha256_provider_dependencies = []

if sha256_provider == 'openssl'
  sha256_provider_sources += files('internal/sha256_openssl.cpp')
  sha256_provider_dependencies += [
    dependency(
      'libcrypto',
      required: true,
      static: want_static,
    ),
  ]
else
  error('unsupported SHA-256 provider: ' + sha256_provider)
endif

identity_dependencies = [
  libpkgsource_dep,
  libpkgplan_dep,
] + sha256_provider_dependencies
adapter_dependencies = [
  libpkgsource_dep,
  libpkgplan_dep,
] + sha256_provider_dependencies

library_link_args = []
library_link_depends = []
if library_kind == 'shared'
  abi_map = join_paths(meson.current_source_dir(), 'libpkgsource-plan.map')
  library_link_args += ['-Wl,--version-script=' + abi_map]
  library_link_depends += files('libpkgsource-plan.map')
endif

identity_sources = files(
  'internal/candidate_control_identity.cpp',
  'internal/source_release_identity.cpp',
)

libpkgsource_plan_identity = static_library(
  'pkgsource-plan-identity',
  identity_sources,
  sha256_provider_sources,
  include_directories: [public_inc, internal_inc],
  dependencies: identity_dependencies,
  cpp_args: visibility_cpp_args,
  gnu_symbol_visibility: 'hidden',
  install: false,
  pic: true,
)

libpkgsource_plan = library(
  'pkgsource-plan',
  'adapter.cpp',
  include_directories: [public_inc, internal_inc],
  dependencies: adapter_dependencies,
  cpp_args: visibility_cpp_args,
  link_args: library_link_args,
  link_depends: library_link_depends,
  link_whole: libpkgsource_plan_identity,
  gnu_symbol_visibility: 'hidden',
  install: true,
  version: meson.project_version(),
  soversion: '1',
)

libpkgsource_plan_dep = declare_dependency(
  link_with: libpkgsource_plan,
  include_directories: public_inc,
  dependencies: [libpkgsource_dep, libpkgplan_dep],
)
meson.override_dependency('libpkgsource-plan', libpkgsource_plan_dep)

install_headers(
  '../include/libpkgsource-plan/libpkgsource-plan.h',
  '../include/libpkgsource-plan/adapter.h',
  '../include/libpkgsource-plan/export.h',
  subdir: 'libpkgsource-plan',
)

pkgconf.generate(
  libpkgsource_plan,
  name: 'libpkgsource-plan',
  filebase: 'libpkgsource-plan',
  description: 'Native libpkgsource to libpkgplan candidate adapter',
  subdirs: '.',
  # The target contributes its full closure as private requirements. Promote
  # public API owners with their dependency objects so Meson de-duplicates
  # them from that closure. The selected SHA-256 provider remains private.
  requires: [libpkgsource_dep, libpkgplan_dep],
)
