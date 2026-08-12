# HTML documentation

## Purpose

The repository can generate one self-contained, versioned HTML documentation
tree for static publication. The tree combines project Markdown, the canonical
manual source, legal files, and Doxygen output for the installed C++ API.

HTML is a derived artifact. Markdown and public headers remain authoritative.
The website publishes the generated tree without reparsing project sources or
reconstructing API documentation.

## Output layout

An enabled build produces:

```text
build/docs/html/libpkgsource-plan/1.1.0/
├── index.html
├── design.html
├── abi.html
├── manual/
├── protocols/
├── history/
├── api/
├── assets/
└── legal/
```

The exact project version is part of the output path. Different releases can be
published beside each other without overwriting historical documentation.

## Build contract

HTML generation is disabled by default. Enable it explicitly with:

```sh
meson setup build -Dhtml_docs=enabled
meson compile -C build html-docs
```

The build requires a supported Pandoc 3.x release and Doxygen. Pandoc converts
reviewed Markdown files. Doxygen reads only installed public headers. The build
fails on converter warnings, missing expected outputs, broken local links,
references to source-only `.md` paths, or absolute source/build paths embedded
in generated pages.

The project-owned CSS files under `docs/assets` are copied into the artifact.
No network resource is required to render the generated tree.

## Installation

When HTML generation is enabled, normal installation places the tree under:

```text
share/htmldocs/libpkgsource-plan/1.1.0
```

The install script uses Meson's `MESON_INSTALL_DESTDIR_PREFIX`, so package
staging with `DESTDIR` or `meson install --destdir` preserves the configured
prefix correctly. The custom `html-docs` installation tag selects only the
rendered site artifact.

## Site publication

A website job may copy the installed version directory into its static document
root and add project-level navigation. It must not rerun Pandoc or Doxygen and
must not alter the versioned artifact in place.
