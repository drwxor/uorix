/* SPDX-License-Identifier: GPL-3.0-only */

#ifndef _STDDEF_H
#define _STDDEF_H

typedef unsigned long size_t;
typedef long ptrdiff_t;

#define NULL ((void *)0)

#define offsetof(type, member) __builtin_offsetof(type, member)

#endif
