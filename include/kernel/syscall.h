/* SPDX-License-Identifier: GPL-3.0-only */

#ifndef UORIX_SYSCALL_H
#define UORIX_SYSCALL_H

#include <stdint.h>

#define SYS_READ   0
#define SYS_WRITE  1
#define SYS_CLEAR  2
#define SYS_EXIT   60

long syscall0(long n);
long syscall1(long n, long a1);
long syscall2(long n, long a1, long a2);
long syscall3(long n, long a1, long a2, long a3);

uint64_t syscall_handler(uint64_t nr, uint64_t a1, uint64_t a2, uint64_t a3, uint64_t a4, uint64_t a5);

#endif
