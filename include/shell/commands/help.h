/* SPDX-License-Identifier: GPL-3.0-only */

#ifndef UORIX_SHELL_HELP_H
#define UORIX_SHELL_HELP_H

#include "user/user.h"

static
void
shell_command_help(void)
{
    user_puts("commands:\n");
    user_puts("  help     show this message\n");
    user_puts("  clear    clear the screen\n");
    user_puts("  echo     print text\n");
    user_puts("  printf   print text without making new line\n");
    user_puts("  uname    show system name\n");
    user_puts("  exit     leave the shell\n");
    user_puts("  info     print info about the system\n");
    user_puts("  mem      show free physical pages\n");
}

#endif
