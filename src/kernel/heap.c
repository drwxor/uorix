/* SPDX-License-Identifier: GPL-3.0-only */

#include "kernel/heap.h"
#include "kernel/pmm.h"
#include "kernel/paging.h"
#include "kernel/renderer.h"

#include <stdint.h>

struct block {
    size_t        size;
    struct block *next;
};

#define HEADER_SIZE sizeof(struct block)
#define MIN_PAYLOAD 16
#define ALIGN 16

static struct block *free_list;
static int heap_ready;

static size_t align_up(size_t n)
{
    return (n + (ALIGN - 1)) & ~(size_t)(ALIGN - 1);
}

static void freelist_add(struct block *b)
{
    struct block *prev = 0;
    struct block *cur  = free_list;

    while (cur && cur < b) {
        prev = cur;
        cur  = cur->next;
    }

    if (cur &&
        (uint8_t *)b + HEADER_SIZE + b->size == (uint8_t *)cur) {
        b->size += HEADER_SIZE + cur->size;
        b->next  = cur->next;
    } else {
        b->next = cur;
    }

    if (prev &&
        (uint8_t *)prev + HEADER_SIZE + prev->size == (uint8_t *)b) {
        prev->size += HEADER_SIZE + b->size;
        prev->next  = b->next;
    } else if (prev) {
        prev->next = b;
    } else {
        free_list = b;
    }
}

static int heap_expand(size_t need)
{
    size_t total = align_up(need + HEADER_SIZE);
    if (total < PAGE_SIZE)
        total = PAGE_SIZE;

    size_t pages = (total + PAGE_SIZE - 1) / PAGE_SIZE;

    for (size_t i = 0; i < pages; i++) {
        uint64_t phys = pmm_alloc_page();
        if (phys == 0)
            return -1;

        struct block *b = (struct block *)paging_phys_to_virt(phys);
        b->size = PAGE_SIZE - HEADER_SIZE;
        freelist_add(b);
    }
    return 0;
}

void heap_init(void)
{
    free_list  = 0;
    heap_ready = 1;

    if (heap_expand(PAGE_SIZE * 4) != 0)
        render_printf("heap: expand failed\n");
    else
        render_printf("heap ready\n");
}

void *kmalloc(size_t size)
{
    if (!heap_ready || size == 0)
        return 0;

    size = align_up(size);
    if (size < MIN_PAYLOAD)
        size = MIN_PAYLOAD;

    for (;;) {
        struct block *prev = 0;
        struct block *cur  = free_list;

        while (cur) {
            if (cur->size >= size) {
                if (cur->size >= size + HEADER_SIZE + MIN_PAYLOAD) {
                    struct block *split =
                        (struct block *)((uint8_t *)cur + HEADER_SIZE + size);
                    split->size = cur->size - size - HEADER_SIZE;
                    split->next = cur->next;
                    cur->size   = size;
                    cur->next   = 0;
                    if (prev)
                        prev->next = split;
                    else
                        free_list = split;
                } else {
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

        if (heap_expand(size) != 0)
            return 0;
    }
}

void kfree(void *ptr)
{
    if (!ptr || !heap_ready)
        return;

    struct block *b = (struct block *)((uint8_t *)ptr - HEADER_SIZE);
    freelist_add(b);
}
