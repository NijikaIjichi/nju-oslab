#include "common.h"
#include "x86.h"
#include "lib.h"

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

int strcmp(const char *s1, const char *s2) {
  while (*s1 != '\0' && *s1 == *s2) {
    s1++;
    s2++;
  }
  return (*(unsigned char *) s1) - (*(unsigned char *) s2);
}

uint32_t strlen(const char *str) {
  uint32_t i;
  for (i = 0; str[i]; ++i) ;
  return i;
}

void memset_uvm(PDE *pgdir, uint32_t dst, int c, uint32_t size) {
  uint32_t fi = dst + size;
  while (dst < fi) *(char*)uvm_walk(pgdir, dst++) = (char)c;
}

void strcpy_uvm(PDE *pgdir, uint32_t dst, const char *src) {
  while (*src) *(char*)uvm_walk(pgdir, dst++) = *src++;
  *(char*)uvm_walk(pgdir, dst) = 0;
}
