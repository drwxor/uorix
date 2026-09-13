# SPDX-License-Identifier: GPL-3.0-only

.section .text
.global user_enter
.type user_enter, @function

user_enter:
    cli

    mov $0x23, %ax
    mov %ax, %ds
    mov %ax, %es

    pushq $0x23
    pushq %rsi
    pushfq
    orq $0x200, (%rsp)
    pushq $0x1B
    pushq %rdi

    iretq

.hang:
    hlt
    jmp .hang
