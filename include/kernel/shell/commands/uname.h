/* SPDX-License-Identifier: GPL-3.0-only */

#ifndef UORIX_SHELL_UNAME_H
#define UORIX_SHELL_UNAME_H

#include "kernel/user/user.h"

static
void
shell_command_uname(void)
{
    user_puts("uorix x86_64\n");
}

#endif
