/* SPDX-License-Identifier: GPL-3.0-only */

#ifndef UORIX_PAGING_H
#define UORIX_PAGING_H

#include <stdint.h>

#define PTE_PRESENT  (1ULL << 0)
#define PTE_WRITE    (1ULL << 1)
#define PTE_USER     (1ULL << 2)
#define PTE_PS       (1ULL << 7)
#define PTE_ADDR_MASK 0x000FFFFFFFFFF000ULL

void paging_allow_user_access(uint64_t hhdm_offset);

#endif
