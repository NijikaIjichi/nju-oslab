#include "lib.h"

int
strcmp (const char *s1,
	const char *s2) {
  while (*s1 != '\0' && *s1 == *s2)
  {
    s1++;
    s2++;
  }

  return (*(unsigned char *) s1) - (*(unsigned char *) s2);
}

char *
strchr (const char *s1,
	int i) {
  const unsigned char *s = (const unsigned char *)s1;
  unsigned char c = i;
  while (*s && *s != c)
    s++;
  if (*s == c)
    return (char *)s;
  return NULL;
}

uint32_t
strlen(const char *s)
{
  int n;

  for(n = 0; s[n]; n++)
    ;
  return n;
}

void memcpy(void *dst, const void *src, uint32_t size) {
  char *_dst = dst, *fi = _dst + size;
  const char *_src = src;
  while (_dst < fi) *_dst++ = *_src++;
}

void memset(void *dst, int c, uint32_t size) {
  char *_dst = dst, *fi = _dst + size;
  while (_dst < fi) *_dst++ = (char)c;
}

void strcpy(char *dst, const char *src) {
  while (*src) *dst++ = *src++;
  *dst = 0;
}
