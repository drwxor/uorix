; SPDX-License-Identifier: GPL-3.0-only

section .text
global _start

_start:
    lea rsp, [rel stack_top]
    and rsp, -16
    call kmain

.hang:
    hlt
    jmp .hang

section .bss
align 16
stack:
    resb 16384
stack_top:
