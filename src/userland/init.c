/* SPDX-License-Identifier: GPL-3.0-only */

#include <stdio.h>
#include <unistd.h>
#include <sys/syscall.h>

int
main(void)
{
    printf("userspace ready\n");
    return 0;
}
