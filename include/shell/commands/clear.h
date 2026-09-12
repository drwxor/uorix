/* SPDX-License-Identifier: GPL-3.0-only */

#ifndef UORIX_SHELL_CLEAR_H
#define UORIX_SHELL_CLEAR_H

#include "kernel/syscall.h"

static void shell_command_clear(void)
{
    syscall0(SYS_CLEAR);
}

#endif
