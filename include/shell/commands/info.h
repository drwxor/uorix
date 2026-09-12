/* SPDX-License-Identifier: GPL-3.0-only */

#ifndef UORIX_SHELL_INFO_H
#define UORIX_SHELL_INFO_H

#include "user/user.h"

static const char *logo[] = {
    "u u  oo  rr  i x x",
    "u u o  o r r i  x ",
    "u u o  o rr  i x x",
    "uuu  oo  r r      ",
    0
};

static void shell_command_info(void)
{
    user_puts("\n");

    for (int i = 0; logo[i] != 0; i++) {
        user_puts(logo[i]);

        if (i == 0)
            user_puts("    uorix x86_64");

        if (i == 1)
            user_puts("    kernel: uorix");

        if (i == 2)
            user_puts("    shell: uorix");

        if (i == 3)
            user_puts("    bootloader: limine");

        user_puts("\n");
    }

    user_puts("\n");
}

#endif
