/* SPDX-License-Identifier: GPL-3.0-only */

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>

int
putchar(int c)
{
    char ch = (char)c;
    if (write(STDOUT_FILENO, &ch, 1) != 1)
        return EOF;
    return (unsigned char)ch;
}

int
puts(const char *s)
{
    size_t n = strlen(s);
    if (write(STDOUT_FILENO, s, n) != (ssize_t)n)
        return EOF;
    if (putchar('\n') == EOF)
        return EOF;
    return 1;
}

int
getchar(void)
{
    char c;
    if (read(STDIN_FILENO, &c, 1) != 1)
        return EOF;
    return (unsigned char)c;
}

char *
fgets(char *s, int size, void *unused)
{
    (void)unused;
    int i = 0;
    if (size <= 0)
        return 0;

    while (i < size - 1) {
        int c = getchar();
        if (c == EOF)
            break;
        s[i++] = (char)c;
        if (c == '\n')
            break;
    }
    if (i == 0)
        return 0;
    s[i] = 0;
    return s;
}

static
void
fmt_uint(char *buf, int *len, unsigned long v, unsigned base)
{
    char tmp[32];
    int n = 0;
    if (v == 0) {
        buf[(*len)++] = '0';
        return;
    }
    while (v) {
        unsigned d = v % base;
        tmp[n++] = (d < 10) ? ('0' + d) : ('a' + d - 10);
        v /= base;
    }
    while (n--)
        buf[(*len)++] = tmp[n];
}

int
vsnprintf(char *out, size_t n, const char *fmt, va_list ap)
{
    char tmp[256];
    int len = 0;
    (void)n;

    while (*fmt && len < (int)sizeof(tmp) - 1)
    {
        if (*fmt != '%')
        {
            tmp[len++] = *fmt++;
            continue;
        }
        fmt++;
        switch (*fmt)
        {
            case '%':
                tmp[len++] = '%';
                break;
            case 'c':
                tmp[len++] = (char)va_arg(ap, int);
                break;
            case 's':
            {
                const char *s = va_arg(ap, const char *);
                if (!s)
                    s = "(null)";
                while (*s && len < (int)sizeof(tmp) - 1)
                    tmp[len++] = *s++;
                break;
            }
            case 'd':
            {
                long v = va_arg(ap, int);
                if (v < 0)
                {
                    tmp[len++] = '-';
                    fmt_uint(tmp, &len, (unsigned long)(-v), 10);
                }
                else
                {
                    fmt_uint(tmp, &len, (unsigned long)v, 10);
                }
                break;
            }
            case 'u':
                fmt_uint(tmp, &len, va_arg(ap, unsigned int), 10);
                break;
            case 'x':
                fmt_uint(tmp, &len, va_arg(ap, unsigned int), 16);
                break;
            case 'p':
            {
                tmp[len++] = '0';
                tmp[len++] = 'x';
                fmt_uint(tmp, &len, (unsigned long)va_arg(ap, void *), 16);
                break;
            }
            default:
                tmp[len++] = '%';
                tmp[len++] = *fmt;
                break;
        }
        if (*fmt)
            fmt++;
    }

    tmp[len] = 0;
    if (out && n) {
        size_t copy = (size_t)len < n - 1 ? (size_t)len : n - 1;
        memcpy(out, tmp, copy);
        out[copy] = 0;
    }
    return len;
}

int
snprintf(char *buf, size_t n, const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    int r = vsnprintf(buf, n, fmt, ap);
    va_end(ap);
    return r;
}

int
vprintf(const char *fmt, va_list ap)
{
    char buf[256];
    int n = vsnprintf(buf, sizeof(buf), fmt, ap);
    if (n < 0)
        return n;
    size_t w = (size_t)n < sizeof(buf) ? (size_t)n : sizeof(buf) - 1;
    write(STDOUT_FILENO, buf, w);
    return n;
}

int
vprintf_colored(const char *fmt, uint32_t color, va_list ap)
{
    char buf[256];
    int n = vsnprintf(buf, sizeof(buf), fmt, ap);
    if (n < 0)
        return n;
    size_t w = (size_t)n < sizeof(buf) ? (size_t)n : sizeof(buf) - 1;
    write_colored(STDOUT_FILENO, buf, w, color);
    return n;
}

int
printf(const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    int r = vprintf(fmt, ap);
    va_end(ap);
    return r;
}

int
printf_colored(const char *fmt, uint32_t color, ...)
{
    va_list ap;
    va_start(ap, color);
    int r = vprintf_colored(fmt, color, ap);
    va_end(ap);
    return r;
}
