/* SPDX-License-Identifier: GPL-3.0-only */

#ifndef UORIX_ATA_H
#define UORIX_ATA_H

#include <stdint.h>

int ata_init(void);
int ata_read_sectors(uint32_t lba, uint8_t count, void *buf);

#endif
