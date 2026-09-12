/* SPDX-License-Identifier: GPL-3.0-only */

#ifndef UORIX_SHELL_EXIT_H
#define UORIX_SHELL_EXIT_H

#include "user/user.h"
#include "kernel/syscall.h"

static void shell_command_exit(void)
{
    user_puts("exiting\n");
    syscall0(SYS_EXIT);
}

#endif
