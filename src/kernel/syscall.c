/* SPDX-License-Identifier: GPL-3.0-only */

#include "kernel/syscall.h"
#include "kernel/renderer.h"
#include "kernel/keyboard.h"
#include "kernel/pmm.h"

#include <stdint.h>

uint64_t
syscall_handler(uint64_t nr, uint64_t a1, uint64_t a2, uint64_t a3, uint64_t a4, uint64_t a5)
{
    (void)a3;
    (void)a4;
    (void)a5;

    switch (nr)
    {
    case SYS_READ:
    {
        char *buf = (char *)a1;
        if (a2 < 1)
            return 0;
        buf[0] = keyboard_getc();
        return 1;
    }

    case SYS_WRITE:
    {
        const char *buf = (const char *)a1;
        uint64_t count  = a2;
        for (uint64_t i = 0; i < count; i++)
            render_putc(buf[i]);
        return count;
    }

    case SYS_CLEAR:
        render_clear(0x00000000);
        return 0;

    case SYS_MEMINFO:
        return pmm_free_pages();

    case SYS_EXIT:
        for (;;)
            __asm__ volatile ("hlt");
        return 0;

    default:
        return (uint64_t)-1;
    }
}

long
syscall0(long n)
{
    long ret;
    __asm__ volatile (
        "int $0x80"
        : "=a"(ret)
        : "a"(n)
        : "memory"
    );
    return ret;
}

long
syscall1(long n, long a1)
{
    long ret;
    __asm__ volatile (
        "int $0x80"
        : "=a"(ret)
        : "a"(n), "D"(a1)
        : "memory"
    );
    return ret;
}

long
syscall2(long n, long a1, long a2)
{
    long ret;
    __asm__ volatile (
        "int $0x80"
        : "=a"(ret)
        : "a"(n), "D"(a1), "S"(a2)
        : "memory"
    );
    return ret;
}

long
syscall3(long n, long a1, long a2, long a3)
{
    long ret;
    __asm__ volatile (
        "int $0x80"
        : "=a"(ret)
        : "a"(n), "D"(a1), "S"(a2), "d"(a3)
        : "memory"
    );
    return ret;
}
