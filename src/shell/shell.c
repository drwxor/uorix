/* SPDX-License-Identifier: GPL-3.0-only */

#include "shell/shell.h"
#include "kernel/renderer.h"
#include "kernel/keyboard.h"

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

static void shell_help(void)
{
    render_printf("uorix shell\n");

    render_printf("commands:\n");
    render_printf("  help     show this message\n");
    render_printf("  clear    clear the screen\n");
    render_printf("  echo     print text\n");
    render_printf("  uname    show system name\n");
}

static void shell_echo(const char *args)
{
    render_printf("%s\n", args);
}

static void shell_execute(void)
{
    if (line_length == 0)
        return;

    if (strcmp_local(line, "help") == 0) {
        shell_help();
        return;
    }

    if (strcmp_local(line, "clear") == 0) {
        render_clear(0x00000000);
        return;
    }

    if (strcmp_local(line, "uname") == 0) {
        render_printf("uorix x86_64\n");
        return;
    }

    if (line_length >= 5 &&
        line[0] == 'e' &&
        line[1] == 'c' &&
        line[2] == 'h' &&
        line[3] == 'o' &&
        line[4] == ' ') {

        shell_echo(line + 5);
        return;
    }

    render_printf(
        "uorix: command not found: %s\n",
        line
    );
}

void shell_init(void)
{
    line_clear();
}

void shell_run(void)
{
    render_printf("$ ");

    for (;;) {
        char c = keyboard_getc();

        if (c == '\n') {
            render_putc('\n');

            shell_execute();
            line_clear();

            render_printf("$ ");
            continue;
        }

        if (c == '\b') {
            if (line_length > 0) {
                line_length--;
                line[line_length] = '\0';

                render_putc('\b');
            }

            continue;
        }

        if ((uint8_t)c < 32)
            continue;

        if (line_length >= LINE_SIZE - 1)
            continue;

        line[line_length++] = c;
        line[line_length] = '\0';

        render_putc(c);
    }
}
