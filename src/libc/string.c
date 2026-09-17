/* SPDX-License-Identifier: GPL-3.0-only */

#include <string.h>
#include <stdint.h>

void *
memcpy(void *dst, const void *src, size_t n)
{
    uint8_t *d = dst;
    const uint8_t *s = src;
    while (n--)
        *d++ = *s++;
    return dst;
}

void *
memmove(void *dst, const void *src, size_t n)
{
    uint8_t *d = dst;
    const uint8_t *s = src;
    if (d < s)
    {
        while (n--)
            *d++ = *s++;
    }
    else
    {
        d += n;
        s += n;
        while (n--)
            *--d = *--s;
    }
    return dst;
}

void *
memset(void *dst, int c, size_t n)
{
    uint8_t *d = dst;
    while (n--)
        *d++ = (uint8_t)c;
    return dst;
}

int
memcmp(const void *a, const void *b, size_t n)
{
    const uint8_t *x = a;
    const uint8_t *y = b;
    while (n--)
    {
        if (*x != *y)
            return (int)*x - (int)*y;
        x++;
        y++;
    }
    return 0;
}

size_t
strlen(const char *s)
{
    const char *p = s;
    while (*p)
        p++;
    return (size_t)(p - s);
}

int
strcmp(const char *a, const char *b)
{
    while (*a && *a == *b)
    {
        a++;
        b++;
    }
    return (unsigned char)*a - (unsigned char)*b;
}

int
strncmp(const char *a, const char *b, size_t n)
{
    if (n == 0)
        return 0;
    while (n > 1 && *a && *a == *b)
    {
        a++;
        b++;
        n--;
    }
    return (unsigned char)*a - (unsigned char)*b;
}

char *
strcpy(char *dst, const char *src)
{
    char *r = dst;
    while ((*dst++ = *src++) != 0)
        ;
    return r;
}

char *
strncpy(char *dst, const char *src, size_t n)
{
    char *r = dst;
    while (n && *src)
    {
        *dst++ = *src++;
        n--;
    }
    while (n--)
        *dst++ = 0;
    return r;
}

char *
strcat(char *dst, const char *src)
{
    char *r = dst;
    while (*dst)
        dst++;
    while ((*dst++ = *src++) != 0)
        ;
    return r;
}

char *
strchr(const char *s, int c)
{
    while (*s)
    {
        if (*s == (char)c)
            return (char *)s;
        s++;
    }
    if ((char)c == 0)
        return (char *)s;
    return 0;
}
