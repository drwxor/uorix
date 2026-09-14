/* SPDX-License-Identifier: GPL-3.0-only */

#ifndef UORIX_EXT2_H
#define UORIX_EXT2_H

#include <stdint.h>

struct ext2_fs;

struct ext2_fs *ext2_mount(uint32_t start_lba);

uint64_t ext2_read_file(struct ext2_fs *fs, const char *path, void **out_buf);
void ext2_unmount(struct ext2_fs *fs);

#endif
