// SPDX-FileCopyrightText: 2026 Alexandr Savca
// SPDX-License-Identifier: GPL-3.0-or-later

/**
 * @file export.h
 * @brief Shared-library visibility for the public libpkgsource-plan ABI.
 */
#pragma once

#if defined(__GNUC__) || defined(__clang__)
#define PKGSOURCE_PLAN_API __attribute__((visibility("default")))
#else
#define PKGSOURCE_PLAN_API
#endif
