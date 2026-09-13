/* SPDX-License-Identifier: GPL-3.0-only */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/syscall.h>

#include "shell.h"


int
main(void)
{
    printf("userspace ready\n");

    return run_shell();
}
