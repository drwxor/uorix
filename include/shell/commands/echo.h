/* SPDX-License-Identifier: GPL-3.0-only */

#ifndef UORIX_SHELL_ECHO_H
#define UORIX_SHELL_ECHO_H

#include "kernel/renderer.h"

static void shell_command_echo(const char *args)
{
    render_printf("%s\n", args);
}

#endif
