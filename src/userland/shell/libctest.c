/* SPDX-License-Identifier: GPL-3.0-only */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int
main(void)
{
    char *p = malloc(32);
    if (p)
    {
        strcpy(p, "hello from C");
        printf("%s (malloc %p)\n", p, (void *)p);
        free(p);
    }
    else
    {
        printf("hello from C\n");
        printf("unable to alloc 32 bytes but aight\n");
    }
    return 0;
}
