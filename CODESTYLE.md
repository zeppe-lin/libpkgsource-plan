# Code style

## Purpose

This repository treats readability as a correctness property. The code should
make ownership, ordering, failure, and authority boundaries visible without
requiring the reader to infer them from convention or historical context.

The style is intentionally conservative. A future edit should be difficult to
misread and difficult to make accidentally unsafe.

## C++ formatting

C++ source and public headers are formatted by `.clang-format` with
`clang-format 17`. The formatter major is pinned because layout is repository
state, not a developer-local preference.

Control statements always use braces, including one-statement bodies. The
braces are not decoration: they prevent comments, logging, or a second statement
from silently changing control flow during maintenance.

Use two-space indentation, no tabs, and an 80-column target. Pointer and
reference symbols bind to the type. Includes are grouped by authority: this
library, local test support, owner libraries, implementation-only third-party
libraries, then the C++ standard library. The formatter sorts names within
each group.

Formatting changes should not be mixed with semantic changes unless the
semantic edit necessarily rewrites the same lines.

## C++ design

Prefer explicit value ownership and RAII. Compute values before moving the
objects from which they are derived. Give protocol constants and identity
framing named declarations instead of repeating string or numeric literals.

Use braces for every `if`, `for`, `while`, and `do` body. Avoid broad `using
namespace` directives. Do not rely on argument-evaluation order, implicit
narrowing, host-width serialization, or enum representation for durable
identity material.

Comments explain invariants, ownership, and non-obvious constraints. They do
not narrate syntax. Public API comments use Doxygen and state inputs, ownership,
return values, and failure behavior that the implementation actually provides.
Do not claim thread safety, compatibility, validation, or persistence guarantees
without executable or owner documentation that proves them.

## Tests

A test name states one contract. Failure output identifies the contract that
failed. Prefer behavioral tests over grepping implementation text. Static
contract tests are reserved for repository boundaries that cannot be observed
through the public API, such as dependency placement, installed headers, and
forbidden adapter dependencies.

Shared fixtures expose semantic options rather than construction noise. Each
excluded or projected fact is varied independently so one field cannot mask
another regression.

## Documentation

Markdown uses ATX headings only: `#`, `##`, `###`, and so on. Setext headings
and horizontal rules are not used. Repository Markdown does not carry SPDX HTML
comments; licensing authority is kept in `COPYING`, `COPYRIGHT`, and source-file
headers where tooling consumes it.

Documentation distinguishes owner facts, adapter behavior, and exclusions. It
must not invent future orchestration, persistence, compatibility, or execution
semantics.

## Review discipline

Every patch should have one review purpose. Mechanical style, semantic code,
tests, and documentation are separated when practical. A release candidate is
accepted only after clean shared and static builds, both supported compilers,
sanitisers, generated metadata checks, installed-consumer checks, manual-page
lint, and exact patch replay.
