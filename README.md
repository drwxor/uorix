# Uorix

Uorix is a small experimental Unix-like operating system for x86_64.

It is built from scratch in C with the goal of being simple, understandable, and fun to work on.

## Status

Uorix is currently in early kernel development.

Current functionality includes:

* Limine-based UEFI boot
* x86_64 kernel
* Framebuffer output
* Built-in bitmap text renderer
* PS/2 keyboard input
* Interactive kernel shell
* Basic shell commands
* Basic serial/debug support

The shell currently runs in the kernel.

## Building

Uorix uses [samu](https://github.com/michaelforney/samurai) with Ninja build files.

Build the kernel with:

```sh
samu
```

The resulting kernel is:

```text
build/uorix.elf
```

## Running

Uorix is currently tested with QEMU and OVMF.

The project includes a small helper script for building, replacing the kernel in the disk image, and starting QEMU.

The helper script is intended for local development and testing only. It contains environment-specific paths and commands and is not currently prepared for use on other systems without modification.

## Shell

The current shell is built directly into the kernel.

Example:

```text
$ help
commands:
  help     show this message
  clear    clear the screen
  echo     print text
  uname    show system name

$ echo hello
hello

$ uname
uorix x86_64
```

## Design

Uorix currently favors a small and direct design.

There is no userspace/process separation yet. Everything currently executes in the kernel address space, including the shell.

This is intentional for the current stage of development. More advanced process isolation may be added later.

## Toolchain

Current development uses:

* Clang
* LLD
* samurai/samu
* QEMU
* OVMF
* Limine

## License

Uorix is free software distributed under the GNU General Public License, version 3.

See `LICENSE` for the full license text.

Individual files contain SPDX license identifiers where appropriate.

## Contributing

Uorix is primarily a personal experimental operating-system project.

Code should stay small, explicit, and easy to understand.

Avoid unnecessary dependencies and complexity.

## Disclaimer

Uorix is experimental software and is not intended for production use.
