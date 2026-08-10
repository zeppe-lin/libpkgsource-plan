#!/bin/sh
# SPDX-FileCopyrightText: 2026 Alexandr Savca
# SPDX-License-Identifier: GPL-3.0-or-later
set -eu
root=$1
fail()
{
  echo "test-layout-contract: $*" >&2
  exit 1
}

for path in \
  unit/content_test.cpp \
  unit/exclusions_test.cpp \
  integration/identity_test.cpp \
  integration/binding_test.cpp \
  protocol/candidate_control_identity_test.cpp \
  protocol/source_release_identity_test.cpp \
  mechanism/sha256_test.cpp \
  header/umbrella_header_test.cpp \
  header/adapter_header_test.cpp \
  installed/consumer.cpp \
  support/source_fixture.h \
  support/test_support.h
do
  [ -f "$root/tests/$path" ] || fail "missing tests/$path"
done

for retired in projection internal public; do
  [ ! -d "$root/tests/$retired" ] || fail "retired tests/$retired remains"
done
