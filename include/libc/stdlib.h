/* SPDX-License-Identifier: GPL-3.0-only */

#ifndef _STDLIB_H
#define _STDLIB_H

#include <stddef.h>

void *malloc(size_t size);
void *calloc(size_t nmemb, size_t size);
void *realloc(void *ptr, size_t size);
void free(void *ptr);
void exit(int status);
void abort(void);
int atoi(const char *s);
long strtol(const char *nptr, char **endptr, int base);

#define EXIT_SUCCESS 0
#define EXIT_FAILURE 1

#endif
