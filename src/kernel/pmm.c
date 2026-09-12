/* SPDX-License-Identifier: GPL-3.0-only */

#include "kernel/pmm.h"
#include "kernel/renderer.h"

#include <stdint.h>

static uint8_t *bitmap;
static uint64_t bitmap_pages;
static uint64_t free_count;
static uint64_t hhdm_offset;

static inline void *phys_to_virt(uint64_t phys)
{
    return (void *)(phys + hhdm_offset);
}

static inline void bitmap_set(uint64_t page)
{
    bitmap[page / 8] |= (uint8_t)(1u << (page % 8));
}

static inline void bitmap_clear(uint64_t page)
{
    bitmap[page / 8] &= (uint8_t)~(1u << (page % 8));
}

static inline int bitmap_test(uint64_t page)
{
    return (bitmap[page / 8] >> (page % 8)) & 1;
}

static void mark_region_used(uint64_t base, uint64_t length)
{
    uint64_t start = base / PAGE_SIZE;
    uint64_t end   = (base + length + PAGE_SIZE - 1) / PAGE_SIZE;

    if (end > bitmap_pages)
        end = bitmap_pages;

    for (uint64_t p = start; p < end; p++) {
        if (!bitmap_test(p)) {
            bitmap_set(p);
            if (free_count > 0)
                free_count--;
        }
    }
}

void pmm_init(struct limine_memmap_response *memmap, uint64_t hhdm)
{
    hhdm_offset = hhdm;
    bitmap = 0;
    bitmap_pages = 0;
    free_count = 0;

    if (memmap == 0 || memmap->entry_count == 0) {
        render_printf("pmm: no memmap\n");
        return;
    }

    uint64_t highest = 0;
    for (uint64_t i = 0; i < memmap->entry_count; i++) {
        struct limine_memmap_entry *e = memmap->entries[i];
        if (e->type != LIMINE_MEMMAP_USABLE)
            continue;
        uint64_t end = e->base + e->length;
        if (end > highest)
            highest = end;
    }

    if (highest == 0) {
        render_printf("pmm: no usable memory\n");
        return;
    }

    bitmap_pages = (highest + PAGE_SIZE - 1) / PAGE_SIZE;
    uint64_t bitmap_bytes = (bitmap_pages + 7) / 8;
    uint64_t bitmap_pages_needed =
        (bitmap_bytes + PAGE_SIZE - 1) / PAGE_SIZE;

    uint64_t bitmap_phys = 0;
    for (uint64_t i = 0; i < memmap->entry_count; i++) {
        struct limine_memmap_entry *e = memmap->entries[i];
        if (e->type != LIMINE_MEMMAP_USABLE)
            continue;
        if (e->length >= bitmap_pages_needed * PAGE_SIZE) {
            bitmap_phys = e->base;
            break;
        }
    }

    if (bitmap_phys == 0) {
        render_printf("pmm: no room for bitmap\n");
        return;
    }

    bitmap = (uint8_t *)phys_to_virt(bitmap_phys);

    for (uint64_t i = 0; i < bitmap_bytes; i++)
        bitmap[i] = 0xFF;
    free_count = 0;

    for (uint64_t i = 0; i < memmap->entry_count; i++) {
        struct limine_memmap_entry *e = memmap->entries[i];
        if (e->type != LIMINE_MEMMAP_USABLE)
            continue;

        uint64_t start = (e->base + PAGE_SIZE - 1) / PAGE_SIZE;
        uint64_t end   = (e->base + e->length) / PAGE_SIZE;

        if (end > bitmap_pages)
            end = bitmap_pages;

        for (uint64_t p = start; p < end; p++) {
            if (bitmap_test(p)) {
                bitmap_clear(p);
                free_count++;
            }
        }
    }

    mark_region_used(bitmap_phys, bitmap_pages_needed * PAGE_SIZE);

    if (bitmap_pages > 0 && !bitmap_test(0)) {
        bitmap_set(0);
        if (free_count > 0)
            free_count--;
    }

    render_printf("pmm: %u pages free / %u total\n", (uint32_t)free_count, (uint32_t)bitmap_pages);
}

uint64_t pmm_alloc_page(void)
{
    if (bitmap == 0 || free_count == 0)
        return 0;

    for (uint64_t p = 1; p < bitmap_pages; p++) {
        if (!bitmap_test(p)) {
            bitmap_set(p);
            free_count--;
            return p * PAGE_SIZE;
        }
    }
    return 0;
}

void pmm_free_page(uint64_t phys)
{
    if (bitmap == 0 || phys == 0)
        return;

    uint64_t p = phys / PAGE_SIZE;
    if (p >= bitmap_pages)
        return;

    if (bitmap_test(p)) {
        bitmap_clear(p);
        free_count++;
    }
}

uint64_t pmm_total_pages(void)
{
    return bitmap_pages;
}

uint64_t pmm_free_pages(void)
{
    return free_count;
}

uint64_t pmm_used_pages(void)
{
    if (bitmap_pages < free_count)
        return 0;
    return bitmap_pages - free_count;
}
