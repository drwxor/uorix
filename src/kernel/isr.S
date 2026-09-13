# SPDX-License-Identifier: GPL-3.0-only

.section .text

.global isr_null
.type isr_null, @function
isr_null:
    cli
    hlt
    jmp isr_null

.global isr_syscall
.type isr_syscall, @function

isr_syscall:
    push %rax
    push %rbx
    push %rcx
    push %rdx
    push %rsi
    push %rdi
    push %rbp
    push %r8
    push %r9
    push %r10
    push %r11
    push %r12
    push %r13
    push %r14
    push %r15

    mov 112(%rsp), %rdi     /* nr  */
    mov  72(%rsp), %rsi     /* a1  */
    mov  80(%rsp), %rdx     /* a2  */
    mov  88(%rsp), %rcx     /* a3  */
    mov  40(%rsp), %r8      /* a4  */
    mov  56(%rsp), %r9      /* a5  */

    mov  %rsp, %rbp
    and  $~0xF, %rsp

    call syscall_handler

    mov  %rax, 112(%rbp)

    mov  %rbp, %rsp

    pop  %r15
    pop  %r14
    pop  %r13
    pop  %r12
    pop  %r11
    pop  %r10
    pop  %r9
    pop  %r8
    pop  %rbp
    pop  %rdi
    pop  %rsi
    pop  %rdx
    pop  %rcx
    pop  %rbx
    pop  %rax

    iretq
