; SPDX-License-Identifier: GPL-3.0-only

section .text
global user_enter

user_enter:
    cli

    mov ax, 0x23
    mov ds, ax
    mov es, ax

    push 0x23
    push rsi
    pushfq
    or qword [rsp], 0x200
    push 0x1B
    push rdi

    iretq

.hang:
    hlt
    jmp .hang
