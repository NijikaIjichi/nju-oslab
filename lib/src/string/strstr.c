#include "lib.h"

char *
strstr (const char *hs, const char *ne)
{
  size_t i;
  int c = ne[0];

  if (c == 0)
    return (char*)hs;

  for ( ; hs[0] != '\0'; hs++)
    {
      if (hs[0] != c)
	continue;
      for (i = 1; ne[i] != 0; i++)
	if (hs[i] != ne[i])
	  break;
      if (ne[i] == '\0')
	return (char*)hs;
    }

  return NULL;
}
