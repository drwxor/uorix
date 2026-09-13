/* SPDX-License-Identifier: GPL-3.0-only */

#include "user/user.h"
#include "kernel/syscall.h"

void
user_putc(char c)
{
    syscall2(SYS_WRITE, (long)&c, 1);
}

void
user_puts(const char *s)
{
    const char *p = s;
    while (*p)
        p++;
    syscall2(SYS_WRITE, (long)s, (long)(p - s));
}

char
user_getc(void)
{
    char c;
    syscall2(SYS_READ, (long)&c, 1);
    return c;
}
