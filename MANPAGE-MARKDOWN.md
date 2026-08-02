# Manual-page Markdown

## Purpose

Manual pages are authored in a restricted Markdown profile and converted to
`man(7)` roff with Pandoc. Markdown is the canonical source. Generated roff is
committed as derived release material so ordinary builds and package consumers
do not require Pandoc.

The profile is deliberately smaller than Pandoc Markdown. A manual page should
read like a manual page in source form, not like a general-purpose document that
happens to have a roff output target.

## Canonical and generated files

Canonical sources live in `man/*.md`. Generated pages live in
`man/generated/` and use their installed manual-page names, including the
section suffix.

Do not edit generated roff directly. Regenerate it with:

```sh
tools/update-man-pages.sh --write
```

Verify that committed output is current with:

```sh
tools/update-man-pages.sh --check
```

Configured Meson trees expose the equivalent `update-man-pages` and
`check-man-pages` targets when Pandoc is available.

The ordinary Meson build copies and installs the committed roff. It does not
invoke Pandoc.

## Document header

Each source starts with one Pandoc title block:

```text
% PAGE_NAME(SECTION) libpkgsource-plan VERSION | libpkgsource-plan
```

The page name is uppercase in the title block. The `NAME` section carries the
lowercase installed name and one concise purpose statement separated by a
literal ASCII hyphen.

## Sections

Use ATX headings only. Top-level manual sections use one `#` and conventional
uppercase names. The normal order is:

```text
# NAME
# SYNOPSIS
# DESCRIPTION
# OWNERSHIP
# IDENTITIES
# ERRORS
# AUTHORITY
# ABI
# SEE ALSO
```

A page may omit an inapplicable section or add a narrowly justified conventional
section. Do not rename a conventional section for prose style.

## Allowed Markdown

The manual profile permits:

- paragraphs;
- unordered and ordered lists;
- definition lists;
- fenced code blocks with an optional language label;
- inline code;
- emphasis where semantic notation requires it.

Use inline code for identifiers, function names, types, literal values, paths,
header names, library names, and manual-page references.

Use fenced code blocks for exact declarations, shell commands, and byte or text
formats. Do not use indentation as a substitute for a fenced block.

## Forbidden Markdown

Manual sources must not contain:

- raw roff;
- raw HTML;
- Setext headings or horizontal rules;
- tables;
- footnotes or citations;
- images;
- task lists;
- automatic or reference links;
- embedded styling directives;
- tabs or trailing whitespace.

These features either obscure the installed representation or make conversion
dependent on a larger Pandoc extension surface than the project needs.

## Conversion contract

The repository generator invokes Pandoc with:

```text
--from=markdown-smart
--to=man
--standalone
--fail-if-warnings
--eol=lf
--wrap=none
```

Pandoc 3.1 or later is required for regeneration. The generator normalizes the
version-bearing Pandoc comment so tool patch releases do not change committed
output solely through provenance text.

Generated roff must pass `mandoc -Tlint`. CI regenerates every page and rejects
any difference from the committed output.

## Review rules

Review the Markdown source and generated roff in the same commit. A semantic
manual change must explain the contract change it documents. A converter-only
change must show why the generated diff is correct.

The generated file is a reproducible release artifact, not an independent
source of truth.
