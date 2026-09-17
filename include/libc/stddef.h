/* SPDX-License-Identifier: GPL-3.0-only */

#ifndef _STDDEF_H
#define _STDDEF_H

typedef unsigned long size_t;
typedef long ptrdiff_t;

#define NULL ((void *)0)

#define offsetof(type, member) __builtin_offsetof(type, member)

#define WHITE_COLOR 0x00FFFFFFu
#define RED_COLOR 0x00FF0000u
#define GREEN_COLOR 0x0000FF00u
#define CYAN_COLOR 0x0000FFFFu

#endif
