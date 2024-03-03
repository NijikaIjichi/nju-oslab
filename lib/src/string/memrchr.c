#include "lib.h"

void *
memrchr (const void *src_void,
      int c,
      size_t length)
{
  const unsigned char *src = (const unsigned char *) src_void + length - 1;
  unsigned char d = c;

  while (length--)
    {
      if (*src == d)
        return (void *) src;
      src--;
    }

  return NULL;
}
