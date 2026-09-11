# SPDX-License-Identifier: GPL-3.0-only

.section .text
.global _start
.type _start, @function

_start:
    lea stack_top(%rip), %rsp
    andq $-16, %rsp
    call kmain

.hang:
    hlt
    jmp .hang

.section .bss
.align 16
stack:
    .skip 16384
stack_top:
