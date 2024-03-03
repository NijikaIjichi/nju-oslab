#include "lib.h"

char *
strcat (char *__restrict s1,
      const char *__restrict s2)
{
  char *s = s1;

  while (*s1)
    s1++;

  while ((*s1++ = *s2++))
    ;
  return s;
}
