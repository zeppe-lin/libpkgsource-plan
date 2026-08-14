# ELF ABI policy

## Purpose

The public C++ declarations and `PKGSOURCE_PLAN_API` annotations define source
intent. The ELF dynamic symbol table is the executable ABI boundary consumed by
already-linked programs.

Compiler visibility alone does not make that boundary exact. Inline and template
material from the C++ standard library can retain default visibility even when
project sources are hidden by default. The shared-library link therefore uses an
anonymous GNU ld export script to localize every symbol not admitted explicitly.

## Canonical manifest

`abi/libpkgsource-plan.exports` is the reviewed ELF ABI manifest for the
GNU/Linux build. It contains exact Itanium C++ ABI symbol names, one per line.

The manifest is authority. The linker script is generated from it during Meson
configuration by `tools/generate-elf-export-script.sh`. Do not edit the generated
script in the build tree.

An addition to the manifest is an ABI addition. A removal or signature-driven
mangling change is an ABI break. Reordering the manifest has no semantic effect,
but keep related constructor, destructor, method, and RTTI symbols grouped for
review.

## Versioning

Version 1.0.0 publishes `libpkgsource-plan.so.1` and one exact symbol set.
Version 2.0.0 publishes `libpkgsource-plan.so.2` with the same adapter-owned
export set because the public API takes and retains `pkgsource::source_snapshot`
by value and therefore must not claim binary compatibility across the
libpkgsource 3 -> 4 carrier generation. It does not publish a named GNU
symbol-version node. A named node is introduced only
when the project has a concrete compatible-extension policy that requires more
than SONAME and exact export control.

The anonymous export script is not an ABI history mechanism. Its only job is to
make the current dynamic surface equal the reviewed manifest.

## Qualification

The shared-library ABI test compares the complete dynamic symbol table with the
manifest. It rejects missing symbols, extra project symbols, standard-library or
private implementation exports, and premature named version nodes.

A public API change must update the header, Doxygen, manual, manifest, behavioral
consumer tests, ABI test expectations, history, and SONAME decision together.
