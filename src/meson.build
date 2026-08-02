# SPDX-FileCopyrightText: 2026 Alexandr Savca
# SPDX-License-Identifier: GPL-3.0-or-later
public_inc = include_directories('../include')

libpkgsource_plan = library(
  'pkgsource-plan',
  'adapter.cpp',
  include_directories: public_inc,
  dependencies: [libpkgsource_dep, libpkgplan_dep, libcrypto_dep],
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
  # them from that closure. libcrypto remains private and appears once.
  requires: [libpkgsource_dep, libpkgplan_dep],
)
