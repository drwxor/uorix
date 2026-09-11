/* SPDX-License-Identifier: GPL-3.0-only */

#ifndef UORIX_SHELL_CLEAR_H
#define UORIX_SHELL_CLEAR_H

#include "kernel/renderer.h"

static void shell_command_clear(void)
{
    render_clear(0x00000000);
}

#endif
