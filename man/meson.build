# SPDX-FileCopyrightText: 2026 Alexandr Savca
# SPDX-License-Identifier: GPL-3.0-or-later
scdoc = find_program('scdoc', required: get_option('man_pages'))
if scdoc.found()
  custom_target(
    'pkgsource_plan_adapter.3',
    input: 'pkgsource_plan_adapter.3.scd',
    output: 'pkgsource_plan_adapter.3',
    command: [scdoc],
    feed: true,
    capture: true,
    install: true,
    install_dir: get_option('mandir') / 'man3',
  )
endif
