/* SPDX-License-Identifier: GPL-3.0-only */

#include "kernel/ata.h"
#include "kernel/io.h"
#include "kernel/renderer.h"

#include <stdint.h>

#define ATA_DATA 0x1F0
#define ATA_ERROR 0x1F1
#define ATA_SECCOUNT 0x1F2
#define ATA_LBA_LO 0x1F3
#define ATA_LBA_MID 0x1F4
#define ATA_LBA_HI 0x1F5
#define ATA_DRIVE 0x1F6
#define ATA_COMMAND 0x1F7
#define ATA_STATUS 0x1F7
#define ATA_ALT_STATUS 0x3F6

#define ATA_SR_BSY 0x80
#define ATA_SR_DRQ 0x08
#define ATA_SR_ERR 0x01

#define ATA_CMD_READ_PIO 0x20

static int ata_present;

static void
ata_wait_bsy(void)
{
    while (inb(ATA_STATUS) & ATA_SR_BSY)
        ;
}

static void
ata_wait_drq(void)
{
    while (!(inb(ATA_STATUS) & ATA_SR_DRQ))
        ;
}

int
ata_init(void)
{
    outb(ATA_ALT_STATUS, 0x04);
    for (volatile int i = 0; i < 10000; i++)
        ;
    outb(ATA_ALT_STATUS, 0x00);
    for (volatile int i = 0; i < 10000; i++)
        ;

    outb(ATA_DRIVE, 0xE0);
    for (volatile int i = 0; i < 1000; i++)
        ;

    uint8_t status = inb(ATA_STATUS);
    if (status == 0xFF)
    {
        render_printf("ata: no device on primary\n");
        ata_present = 0;
        return -1;
    }

    ata_present = 1;
    render_printf("ata: primary master present (status 0x%x)\n", status);
    return 0;
}

int
ata_read_sectors(uint32_t lba, uint8_t count, void *buf)
{
    if (!ata_present || count == 0)
        return -1;

    ata_wait_bsy();

    outb(ATA_DRIVE, 0xE0 | ((lba >> 24) & 0x0F));
    outb(ATA_SECCOUNT, count);
    outb(ATA_LBA_LO, (uint8_t)(lba));
    outb(ATA_LBA_MID, (uint8_t)(lba >> 8));
    outb(ATA_LBA_HI, (uint8_t)(lba >> 16));
    outb(ATA_COMMAND, ATA_CMD_READ_PIO);

    uint8_t *dst = (uint8_t *)buf;

    for (uint8_t s = 0; s < count; s++)
    {
        ata_wait_bsy();
        if (inb(ATA_STATUS) & ATA_SR_ERR)
        {
            render_printf("ata: read error at LBA %u\n", lba + s);
            return -1;
        }
        ata_wait_drq();
        insw(ATA_DATA, dst, 256);
        dst += 512;
    }

    return 0;
}
