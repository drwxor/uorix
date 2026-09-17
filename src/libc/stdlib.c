/* SPDX-License-Identifier: GPL-3.0-only */

#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>

struct block {
    size_t size;
    struct block *next;
};

#define HEADER_SIZE sizeof(struct block)
#define ALIGN 16
#define MIN_PAYLOAD 16

static struct block *free_list;

static
size_t
align_up(size_t n)
{
    return (n + (ALIGN - 1)) & ~(size_t)(ALIGN - 1);
}

static
void
freelist_add(struct block *b)
{
    struct block *prev = 0;
    struct block *cur = free_list;

    while (cur && cur < b)
    {
        prev = cur;
        cur = cur->next;
    }

    if (cur && (uint8_t *)b + HEADER_SIZE + b->size == (uint8_t *)cur)
    {
        b->size += HEADER_SIZE + cur->size;
        b->next = cur->next;
    }
    else
    {
        b->next = cur;
    }

    if (prev && (uint8_t *)prev + HEADER_SIZE + prev->size == (uint8_t *)b)
    {
        prev->size += HEADER_SIZE + b->size;
        prev->next = b->next;
    }
    else if (prev)
    {
        prev->next = b;
    }
    else
    {
        free_list = b;
    }
}

void *
malloc(size_t size)
{
    if (size == 0)
        return 0;

    size = align_up(size);
    if (size < MIN_PAYLOAD)
        size = MIN_PAYLOAD;

    for (;;)
    {
        struct block *prev = 0;
        struct block *cur = free_list;

        while (cur)
        {
            if (cur->size >= size)
            {
                if (cur->size >= size + HEADER_SIZE + MIN_PAYLOAD)
                {
                    struct block *split = (struct block *)((uint8_t *)cur + HEADER_SIZE + size);
                    split->size = cur->size - size - HEADER_SIZE;
                    split->next = cur->next;
                    cur->size = size;
                    cur->next = 0;
                    if (prev)
                        prev->next = split;
                    else
                        free_list = split;
                }
                else
                {
                    if (prev)
                        prev->next = cur->next;
                    else
                        free_list = cur->next;
                    cur->next = 0;
                }
                return (void *)((uint8_t *)cur + HEADER_SIZE);
            }
            prev = cur;
            cur  = cur->next;
        }

        size_t need = size + HEADER_SIZE;
        if (need < 4096)
            need = 4096;
        void *p = sbrk((intptr_t)need);
        if (p == (void *)-1)
            return 0;

        struct block *b = (struct block *)p;
        b->size = need - HEADER_SIZE;
        b->next = 0;
        freelist_add(b);
    }
}

void *
calloc(size_t nmemb, size_t size)
{
    size_t n = nmemb * size;
    void *p = malloc(n);
    if (p)
        memset(p, 0, n);
    return p;
}

void *
realloc(void *ptr, size_t size)
{
    if (ptr == 0)
        return malloc(size);
    if (size == 0)
    {
        free(ptr);
        return 0;
    }

    struct block *b = (struct block *)((uint8_t *)ptr - HEADER_SIZE);
    if (b->size >= size)
    {
        return ptr;
    }

    void *n = malloc(size);
    if (n == 0)
        return 0;
    memcpy(n, ptr, b->size < size ? b->size : size);
    free(ptr);
    return n;
}

void
free(void *ptr)
{
    if (ptr == 0)
        return;
    struct block *b = (struct block *)((uint8_t *)ptr - HEADER_SIZE);
    freelist_add(b);
}

void
exit(int status)
{
    _exit(status);
}

void
abort(void)
{
    write(2, "abort\n", 6);
    _exit(134);
}

int
atoi(const char *s)
{
    int sign = 1;
    int n = 0;
    if (*s == '-')
    {
        sign = -1;
        s++;
    }
    while (*s >= '0' && *s <= '9')
    {
        n = n * 10 + (*s - '0');
        s++;
    }
    return sign * n;
}

long
strtol(const char *nptr, char **endptr, int base)
{
    const char *s = nptr;
    long sign = 1;
    long n = 0;

    if (base == 0)
        base = 10;

    if (*s == '-')
    {
        sign = -1;
        s++;
    }
    else if (*s == '+')
    {
        s++;
    }

    while (*s)
    {
        int d;
        if (*s >= '0' && *s <= '9')
            d = *s - '0';
        else if (*s >= 'a' && *s <= 'z')
            d = *s - 'a' + 10;
        else if (*s >= 'A' && *s <= 'Z')
            d = *s - 'A' + 10;
        else
            break;
        if (d >= base)
            break;
        n = n * base + d;
        s++;
    }

    if (endptr)
        *endptr = (char *)s;
    return sign * n;
}
