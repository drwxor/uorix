/* SPDX-License-Identifier: GPL-3.0-only */

#include "shell/shell.h"

#include "user/user.h"

#include "shell/commands/clear.h"
#include "shell/commands/help.h"
#include "shell/commands/info.h"
#include "shell/commands/uname.h"
#include "shell/commands/exit.h"
#include "shell/commands/mem.h"

#include <stdint.h>

#define LINE_SIZE 128

static char line[LINE_SIZE];
static uint64_t line_length;

static int strcmp_local(const char *a, const char *b)
{
    while (*a && *a == *b) {
        a++;
        b++;
    }
    return (unsigned char)*a - (unsigned char)*b;
}

static void line_clear(void)
{
    line_length = 0;
    line[0] = '\0';
}

static void shell_execute(void)
{
    if (line_length == 0)
        return;

    if (strcmp_local(line, "help") == 0) {
        shell_command_help();
        return;
    }

    if (strcmp_local(line, "clear") == 0) {
        shell_command_clear();
        return;
    }

    if (strcmp_local(line, "uname") == 0) {
        shell_command_uname();
        return;
    }

    if (strcmp_local(line, "exit") == 0) {
        shell_command_exit();
        return;
    }

    if (strcmp_local(line, "info") == 0) {
        shell_command_info();
        return;
    }

    if (strcmp_local(line, "mem") == 0) {
        shell_command_mem();
        return;
    }

    if (line_length >= 5 &&
        line[0] == 'e' &&
        line[1] == 'c' &&
        line[2] == 'h' &&
        line[3] == 'o' &&
        line[4] == ' ') {
        user_puts(line + 5);
        user_putc('\n');
        return;
    }

    user_puts("uorix: command not found: ");
    user_puts(line);
    user_putc('\n');
}

void shell_init(void)
{
    line_clear();
}

void shell_run(void)
{
    user_puts("$ ");

    for (;;) {
        char c = user_getc();

        if (c == '\n') {
            user_putc('\n');
            shell_execute();
            line_clear();
            user_puts("$ ");
            continue;
        }

        if (c == '\b') {
            if (line_length > 0) {
                line_length--;
                line[line_length] = '\0';
                user_putc('\b');
            }
            continue;
        }

        if ((uint8_t)c < 32)
            continue;

        if (line_length >= LINE_SIZE - 1)
            continue;

        line[line_length++] = c;
        line[line_length] = '\0';
        user_putc(c);
    }
}
