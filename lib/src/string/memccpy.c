#include "lib.h"

void *
memccpy (void *__restrict dst0,
      const void *__restrict src0,
      int endchar0,
      size_t len0)
{
  void *ptr = NULL;
  char *dst = (char *) dst0;
  char *src = (char *) src0;
  char endchar = endchar0 & 0xff;

  while (len0--)
    {
      if ((*dst++ = *src++) == endchar)
        {
          ptr = dst;
          break;
        }
    }

  return ptr;
}
