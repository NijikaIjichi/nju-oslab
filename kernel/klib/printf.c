#include "klib.h"
#include "serial.h"

void putstrn(const char *str, size_t count) {
  for (int i = 0; i < count; ++i) {
    putchar(str[i]);
  }
}

void putstr(const char *str) {
  putstrn(str, strlen(str));
}

int printf(const char *format, ...) {
  int r;
  va_list args;
  va_start(args, format);
  r = vcprintf(putstr, format, args);
  va_end(args);
  return r;
}
