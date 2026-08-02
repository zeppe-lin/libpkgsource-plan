#!/bin/sh
# SPDX-FileCopyrightText: 2026 Alexandr Savca
# SPDX-License-Identifier: GPL-3.0-or-later
set -eu

root=$1

fail()
{
  echo "owner-boundary-test: $*" >&2
  exit 1
}

public_header=$root/include/libpkgsource-plan/adapter.h
adapter=$root/src/adapter.cpp
identity=$root/src/internal/candidate_control_identity.cpp
provider=$root/src/internal/sha256_openssl.cpp
build=$root/meson.build
source_build=$root/src/meson.build
options=$root/meson.options

for required in \
  '<libpkgsource/snapshot.h>' \
  '<libpkgplan/package_fact.h>'
do
  grep -F "$required" "$public_header" >/dev/null ||
    fail "public API omits owner header $required"
done

grep -F "'libpkgsource'" "$build" >/dev/null ||
  fail 'libpkgsource owner dependency is missing'
grep -F "'libpkgplan'" "$build" >/dev/null ||
  fail 'libpkgplan owner dependency is missing'
grep -F "'sha256_provider'" "$options" >/dev/null ||
  fail 'SHA-256 provider selection option is missing'
grep -F "'openssl'" "$options" >/dev/null ||
  fail 'OpenSSL provider is not an admitted option'
grep -F "'libcrypto'" "$source_build" >/dev/null ||
  fail 'OpenSSL provider dependency is missing'
grep -F "internal/sha256_openssl.cpp" "$source_build" >/dev/null ||
  fail 'OpenSSL provider source is not selected explicitly'

if grep -R -E '#include <(yaml|libpkgsource-codec|libpkgimage)' \
    "$root/include" "$root/src" >/dev/null; then
  fail 'foreign parser, codec, or image authority crosses adapter source'
fi

if grep -R -E 'dependency\(.?(yaml|libpkgsource-codec|libpkgimage)' \
    "$root/meson.build" "$source_build" >/dev/null; then
  fail 'foreign parser, codec, or image dependency crosses adapter build'
fi

if grep -R -E 'parse_(recipe|profiles)|seal_recipe_yaml|yaml_parser' \
    "$root/include" "$adapter" >/dev/null; then
  fail 'source-syntax parsing crosses the planner adapter boundary'
fi

if grep -E 'openssl|EVP_|SHA256' "$adapter" "$public_header" "$identity" \
    >/dev/null; then
  fail 'cryptographic provider details escaped their internal provider file'
fi

provider_headers=$(grep -R -l '#include <openssl/' "$root/src" || true)
[ "$provider_headers" = "$provider" ] ||
  fail 'OpenSSL headers appear outside the selected provider implementation'

grep -F '#include <libpkgsource-plan/export.h>' "$public_header" >/dev/null ||
  fail 'public API does not bind explicit visibility'
grep -F 'PKGSOURCE_PLAN_API' "$public_header" >/dev/null ||
  fail 'public declarations are not exported explicitly'

grep -F 'pkgsource::source_snapshot source_' "$public_header" >/dev/null ||
  fail 'projection does not retain complete source authority'

grep -F 'catch (const internal::identity_error& error)' "$adapter" >/dev/null ||
  fail 'internal identity failures are not translated at the adapter boundary'
grep -F 'catch (const pkgplan::fact_error& error)' "$adapter" >/dev/null ||
  fail 'planner validation is not translated through pkgplan::fact_error'
if grep -F 'catch (const std::exception&' "$adapter" >/dev/null; then
  fail 'adapter broadly reclassifies unrelated standard exceptions'
fi
