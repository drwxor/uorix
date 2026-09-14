# SPDX-License-Identifier: GPL-3.0-only

.section .rodata
.align 16
.global init_elf_start
.global init_elf_end
init_elf_start:
    .incbin "build/userland/init.elf"
init_elf_end:
