/* SPDX-License-Identifier: GPL-3.0-only */

#ifndef _STDINT_H
#define _STDINT_H

typedef signed char int8_t;
typedef short int16_t;
typedef int int32_t;
typedef long int64_t;

typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;
typedef unsigned long uint64_t;

typedef uint64_t uintptr_t;
typedef int64_t intptr_t;

#define INT64_MAX  9223372036854775807L
#define UINT64_MAX 18446744073709551615UL

#endif
