/* SPDX-License-Identifier: GPL-3.0-only */

#include <stdio.h>

int 
main(void) 
{
  while (1) 
  {
    if (argc > 1) 
    {
      printf("%s\n", argv[1]);
    } 
    else 
    {
      printf("y\n");
    }
  }
}
