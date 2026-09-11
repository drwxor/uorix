/* SPDX-License-Identifier: GPL-3.0-only */

#ifndef UORIX_SHELL_INFO_H
#define UORIX_SHELL_INFO_H

#include "kernel/renderer.h"

static const char *logo[] = {
    "u u  oo  rr  i x x",
    "u u o  o r r i  x ",
    "u u o  o rr  i x x",
    "uuu  oo  r r      ",
    0
};

static void shell_command_info(void)
{
    render_printf("\n");

    for (int i = 0; logo[i] != 0; i++) {
        render_printf("%s", logo[i]);

        if (i == 0)
            render_printf("    uorix x86_64");

        if (i == 1)
            render_printf("    kernel: uorix");

        if (i == 2)
            render_printf("    shell: uorix");

        if (i == 3)
            render_printf("    bootloader: limine");

        render_printf("\n");
    }

    render_printf("\n");
}

#endif
