/* SPDX-License-Identifier: GPL-3.0-only */

#include "kernel/gdt.h"

static struct gdt_entry gdt[7];
static struct gdt_tss_entry *tss_entry = (struct gdt_tss_entry *)&gdt[5];
static struct gdt_ptr gdtr;
static struct tss tss;

static void
gdt_set_entry(int idx, uint32_t base, uint32_t limit, uint8_t access, uint8_t gran)
{
    gdt[idx].base_low = base & 0xFFFF;
    gdt[idx].base_mid = (base >> 16) & 0xFF;
    gdt[idx].base_high = (base >> 24) & 0xFF;
    gdt[idx].limit_low = limit & 0xFFFF;
    gdt[idx].granularity = (limit >> 16) & 0x0F;
    gdt[idx].granularity |= gran & 0xF0;
    gdt[idx].access = access;
}

static void
gdt_set_tss(uint64_t base, uint32_t limit)
{
    tss_entry->limit_low = limit & 0xFFFF;
    tss_entry->base_low = base & 0xFFFF;
    tss_entry->base_mid = (base >> 16) & 0xFF;
    tss_entry->access = 0x89;
    tss_entry->granularity = (limit >> 16) & 0x0F;
    tss_entry->base_high = (base >> 24) & 0xFF;
    tss_entry->base_upper = (base >> 32) & 0xFFFFFFFF;
    tss_entry->reserved = 0;
}

void
tss_set_rsp0(uint64_t rsp0)
{
    tss.rsp0 = rsp0;
}

void
gdt_init(void)
{
    gdt_set_entry(0, 0, 0, 0, 0);

    gdt_set_entry(1, 0, 0xFFFFF, 0x9A, 0xA0);
    gdt_set_entry(2, 0, 0xFFFFF, 0x92, 0xC0);
    gdt_set_entry(3, 0, 0xFFFFF, 0xFA, 0xA0);
    gdt_set_entry(4, 0, 0xFFFFF, 0xF2, 0xC0);

    gdt_set_tss((uint64_t)&tss, sizeof(tss) - 1);

    gdtr.limit = sizeof(gdt) - 1;
    gdtr.base = (uint64_t)&gdt;

    __asm__ volatile (
        "lgdt %0"
        :
        : "m"(gdtr)
        : "memory"
    );

    __asm__ volatile (
        "mov $0x10, %%ax\n"
        "mov %%ax, %%ds\n"
        "mov %%ax, %%es\n"
        "mov %%ax, %%ss\n"
        "mov %%ax, %%fs\n"
        "mov %%ax, %%gs\n"
        :
        :
        : "ax", "memory"
    );

    __asm__ volatile (
        "pushq $0x08\n"
        "leaq 1f(%%rip), %%rax\n"
        "pushq %%rax\n"
        "lretq\n"
        "1:\n"
        :
        :
        : "rax", "memory"
    );

    __asm__ volatile (
        "mov $0x28, %%ax\n"
        "ltr %%ax\n"
        :
        :
        : "ax", "memory"
    );
}
