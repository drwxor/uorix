/* SPDX-License-Identifier: GPL-3.0-only */

#include "kernel/syscall.h"
#include "kernel/renderer.h"
#include "kernel/driver/keyboard.h"
#include "kernel/pmm.h"
#include "kernel/elf.h"
#include "kernel/fs/ext2.h"
#include "kernel/paging.h"
#include "kernel/heap.h"

#include <stdint.h>

extern void user_enter(uint64_t entry, uint64_t user_stack)
    __attribute__((noreturn));

static
int
copy_user_string(char *dst, uint64_t cap, uint64_t src)
{
    if (src < 0x1000 || src >= USER_LIMIT)
        return -1;

    for (uint64_t i = 0; i + 1 < cap; i++)
    {
        uint64_t va = src + i;

        if (va >= USER_LIMIT)
            return -1;

        uint64_t phys = paging_virt_to_phys(va);
        if (phys == 0)
            return -1;

        dst[i] = *(const char *)paging_phys_to_virt(phys);

        if (dst[i] == '\0')
            return 0;
    }

    dst[cap - 1] = '\0';
    return -1;
}

long
sys_exec(const char *user_path)
{
    char path[128];

    if (copy_user_string(path, sizeof(path), (uint64_t)user_path) != 0)
        return -1;

    if (!rootfs)
    {
        return -1;
    }

    void *file_buf = 0;

    uint64_t file_size = ext2_read_file(rootfs, path, &file_buf);

    if (file_size == (uint64_t)-1 || file_buf == 0)
        return -1;

    uint64_t user_stack_top = 0;

    uint64_t user_pml4 = paging_create_user_as(&user_stack_top);

    if (user_pml4 == 0)
    {
        kfree(file_buf);
        return -1;
    }

    uint64_t entry = 0;
    uint64_t brk = 0;

    int rc = elf_load(
        file_buf,
        file_size,
        user_pml4,
        &entry,
        &brk
    );

    kfree(file_buf);

    if (rc != 0)
        return -1;

    elf_brk_init(brk, user_pml4);

    paging_load_cr3(user_pml4);

    user_enter(entry, user_stack_top);

    __builtin_unreachable();
}

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
            uint64_t count = a2;
            for (uint64_t i = 0; i < count; i++)
                render_putc(buf[i], a3);
            return count;
        }

        case SYS_CLEAR:
            render_clear(0x00000000);
            return 0;

        case SYS_MEMINFO:
            return pmm_free_pages();

        case SYS_BRK:
            return elf_brk(a1);

        case SYS_EXEC:
            return sys_exec((const char *)a1);

        case SYS_EXIT:
            render_printf("\n[pid 1] exit %u\n", a1);
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
