/* SPDX-License-Identifier: GPL-3.0-only */

#ifndef _STDIO_H
#define _STDIO_H

#include <stddef.h>
#include <stdarg.h>
#include <sys/types.h>

#define EOF (-1)

int putchar(int c);
int puts(const char *s);
int getchar(void);
char *fgets(char *s, int size, void *unused);
int printf(const char *fmt, ...);
int printf_colored(const char *fmt, uint32_t color, ...);
int vprintf(const char *fmt, va_list ap);
int snprintf(char *buf, size_t n, const char *fmt, ...);
int vsnprintf(char *buf, size_t n, const char *fmt, va_list ap);

#endif
