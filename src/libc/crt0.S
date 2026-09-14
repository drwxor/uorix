# SPDX-License-Identifier: GPL-3.0-only

.section .note.GNU-stack,"",@progbits

.section .text
.global _start
.type _start, @function

_start:
    xor  %rbp, %rbp
    and  $~0xF, %rsp
    call main
    mov  %eax, %edi
    mov  $60, %eax
    int  $0x80
.hang:
    hlt
    jmp  .hang
