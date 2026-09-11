/* SPDX-License-Identifier: GPL-3.0-only */

#ifndef UORIX_SHELL_HELP_H
#define UORIX_SHELL_HELP_H

#include "kernel/renderer.h"

static void shell_command_help(void)
{
    render_printf("commands:\n");
    render_printf("  help     show this message\n");
    render_printf("  clear    clear the screen\n");
    render_printf("  echo     print text\n");
    render_printf("  uname    show system name\n");
}

#endif
