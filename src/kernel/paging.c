/* SPDX-License-Identifier: GPL-3.0-only */

#include "kernel/paging.h"
#include "kernel/renderer.h"
#include "kernel/limine.h"

#include <stdint.h>

#define PTE_PRESENT  (1ULL << 0)
#define PTE_USER     (1ULL << 2)
#define PTE_PS       (1ULL << 7)
#define PTE_ADDR_MASK 0x000FFFFFFFFFF000ULL

static uint64_t hhdm_offset;

static inline uint64_t read_cr3(void)
{
    uint64_t cr3;
    __asm__ volatile ("mov %%cr3, %0" : "=r"(cr3));
    return cr3;
}

static inline void *phys_to_virt(uint64_t phys)
{
    return (void *)(phys + hhdm_offset);
}

static void mark_user(uint64_t *pte)
{
    if (*pte & PTE_PRESENT)
        *pte |= PTE_USER;
}

void paging_allow_user_access(uint64_t hhdm)
{
    hhdm_offset = hhdm;

    uint64_t cr3 = read_cr3();
    uint64_t *pml4 = (uint64_t *)phys_to_virt(cr3 & PTE_ADDR_MASK);

    for (int i = 0; i < 512; i++) {
        if (!(pml4[i] & PTE_PRESENT))
            continue;
        mark_user(&pml4[i]);

        uint64_t *pdpt = (uint64_t *)phys_to_virt(pml4[i] & PTE_ADDR_MASK);
        for (int j = 0; j < 512; j++) {
            if (!(pdpt[j] & PTE_PRESENT))
                continue;
            mark_user(&pdpt[j]);

            if (pdpt[j] & PTE_PS)
                continue;

            uint64_t *pd = (uint64_t *)phys_to_virt(pdpt[j] & PTE_ADDR_MASK);
            for (int k = 0; k < 512; k++) {
                if (!(pd[k] & PTE_PRESENT))
                    continue;
                mark_user(&pd[k]);

                if (pd[k] & PTE_PS)
                    continue;

                uint64_t *pt = (uint64_t *)phys_to_virt(pd[k] & PTE_ADDR_MASK);
                for (int l = 0; l < 512; l++) {
                    if (pt[l] & PTE_PRESENT)
                        mark_user(&pt[l]);
                }
            }
        }
    }

    __asm__ volatile (
        "mov %%cr3, %%rax\n"
        "mov %%rax, %%cr3\n"
        :
        :
        : "rax", "memory"
    );

    // render_printf("paging: user bit set (hhdm=%x)\n", hhdm);
}
