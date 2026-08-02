// SPDX-FileCopyrightText: 2026 Alexandr Savca
// SPDX-License-Identifier: GPL-3.0-or-later

/**
 * @file adapter.h
 * @brief Projection from sealed source authority to planner candidate facts.
 */
#pragma once

#include <libpkgplan/package_fact.h>
#include <libpkgsource/snapshot.h>

#include <stdexcept>
#include <string>

namespace pkgsource::plan_adapter {

/** @brief Machine-readable category for a projection failure. */
enum class projection_error_code {
  /** Candidate or package-release identity material could not be produced. */
  identity,

  /** A value could not be represented by the planner owner API. */
  planner_fact,
};

/**
 * @brief Typed failure raised while projecting one source snapshot.
 *
 * The exception preserves a stable category for callers and a diagnostic
 * message for operators. The adapter does not translate owner-library error
 * types into additional policy categories.
 */
class projection_error final : public std::runtime_error {
public:
  /**
   * @brief Construct a projection failure.
   * @param code Stable failure category.
   * @param message Human-readable diagnostic text.
   */
  projection_error(projection_error_code code, std::string message);

  /** @return The stable category supplied at construction. */
  [[nodiscard]] projection_error_code code() const noexcept;

private:
  projection_error_code code_;
};

/**
 * @brief Planner candidate retained beside its complete issuing source.
 *
 * The planner fact is a deliberately narrow projection. Retaining the sealed
 * source snapshot prevents that projection from erasing the authority and
 * identity from which the fact was derived.
 */
class candidate_projection final {
public:
  /**
   * @brief Bind a planner candidate to its issuing source snapshot.
   * @param source Complete sealed source authority, taken by value.
   * @param candidate Planner fact derived from @p source, taken by value.
   */
  candidate_projection(pkgsource::source_snapshot source,
                       pkgplan::candidate_package_fact candidate);

  /** @return The complete retained source snapshot. */
  [[nodiscard]] const pkgsource::source_snapshot& source() const noexcept;

  /** @return The identity of the retained source snapshot. */
  [[nodiscard]] const pkgsource::source_snapshot_identity&
  source_identity() const noexcept;

  /** @return The projected planner candidate fact. */
  [[nodiscard]] const pkgplan::candidate_package_fact&
  candidate() const noexcept;

private:
  pkgsource::source_snapshot source_;
  pkgplan::candidate_package_fact candidate_;
};

/**
 * @brief Project planner-owned candidate facts from sealed source authority.
 *
 * The projection transfers normalized runtime package requirements, removal
 * lifecycle programs, target architecture control, package release
 * coordinates, and the source-issued package release identity. It computes
 * candidate-control identity over the resulting planner-normalized control.
 *
 * Build and check requirements, lifecycle requirements, selected profiles,
 * build architecture constraints, source inputs, build and check programs,
 * installation lifecycle programs, source origin, and source identity remain
 * only in the retained source snapshot.
 *
 * @param source Complete sealed source snapshot. The returned object owns this
 * snapshot; pass an rvalue to avoid a copy.
 * @return The projected planner fact and its retained issuing source.
 * @throws projection_error If identity material cannot be produced or an owner
 * value cannot be represented by the planner API.
 */
[[nodiscard]] candidate_projection
project_candidate(pkgsource::source_snapshot source);

} // namespace pkgsource::plan_adapter
