# SPDX-FileCopyrightText: 2026 Alexandr Savca
# SPDX-License-Identifier: GPL-3.0-or-later
project_docdir = get_option('datadir') / 'doc' / meson.project_name()

install_data(
  '../README.md',
  '../HISTORY.md',
  '../CONTRIBUTING.md',
  '../MAINTAINING.md',
  '../COPYING',
  '../COPYRIGHT',
  install_dir: project_docdir,
  install_tag: 'doc',
)

install_data(
  'architecture.md',
  'abi.md',
  'code-style.md',
  'manpage-markdown.md',
  'testing.md',
  install_dir: project_docdir,
  install_tag: 'doc',
)

install_data(
  'protocols/candidate-control-identity-v1.md',
  install_dir: project_docdir / 'protocols',
  install_tag: 'doc',
)

install_data(
  'history/in-tree-adapter-migration.md',
  install_dir: project_docdir / 'history',
  install_tag: 'doc',
)

install_data(
  'man/pkgsource_plan_adapter.3.md',
  install_dir: project_docdir / 'man',
  install_tag: 'doc',
)

subdir('man')
