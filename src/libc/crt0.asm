; SPDX-License-Identifier: GPL-3.0-only

section .note.GNU-stack noalloc noexec nowrite progbits

section .text
global _start

_start:
    xor rbp, rbp
    and rsp, -16
    call main
    mov edi, eax
    mov eax, 60
    int 0x80
.hang:
    hlt
    jmp .hang
