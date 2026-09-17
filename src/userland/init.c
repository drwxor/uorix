/* SPDX-License-Identifier: GPL-3.0-only */

#include <stdio.h>
#include <unistd.h>
#include <stddef.h>
#include <sys/syscall.h>

int
main(void)
{
    printf("userspace "); printf_colored("[OK]\n", GREEN_COLOR);

    printf_colored("welcome to uorix!\n", CYAN_COLOR);

    exec("/bin/sh");

    return 0;
}
