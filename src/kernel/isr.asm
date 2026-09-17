; SPDX-License-Identifier: GPL-3.0-only

section .text

global isr_null
isr_null:
    cli
    hlt
    jmp isr_null

global isr_syscall
isr_syscall:
    push rax
    push rbx
    push rcx
    push rdx
    push rsi
    push rdi
    push rbp
    push r8
    push r9
    push r10
    push r11
    push r12
    push r13
    push r14
    push r15

    mov rdi, [rsp+112]
    mov rsi, [rsp+72]
    mov rdx, [rsp+80]
    mov rcx, [rsp+88]
    mov r8,  [rsp+40]
    mov r9,  [rsp+56]

    mov rbp, rsp
    and rsp, -16

    call syscall_handler

    mov [rbp+112], rax

    mov rsp, rbp

    pop r15
    pop r14
    pop r13
    pop r12
    pop r11
    pop r10
    pop r9
    pop r8
    pop rbp
    pop rdi
    pop rsi
    pop rdx
    pop rcx
    pop rbx
    pop rax

    iretq
