#include "ulib.h"

#define SYSCALL_PUTSTR // TODO: uncomment me at Lab1-5

#ifndef SYSCALL_PUTSTR

#define SERIAL_PORT 0x3F8

static inline uint8_t inb(int port) {
  uint8_t data;
  asm volatile ("inb %1, %0" : "=a"(data) : "d"((uint16_t)port));
  return data;
}

static inline void outb(int port, uint8_t data) {
  asm volatile ("outb %%al, %%dx" : : "a"(data), "d"((uint16_t)port));
}

static inline bool serial_idle() {
  return (inb(SERIAL_PORT + 5) & 0x20) != 0;
}

static inline void putchar(char ch) {
  while (!serial_idle()) ;
  outb(SERIAL_PORT, ch);
}

void putstr(const char *str) {
  for (; *str; ++str) {
    putchar(*str);
  }
}

#else

void putstr(const char *str) {
  write(1, str, strlen(str));
}

#endif

int printf(const char *format, ...) {
  int r;
  va_list args;
  va_start(args, format);
  r = vcprintf(putstr, format, args);
  va_end(args);
  return r;
}

int fprintf(int fd, const char *format, ...) {
  int r;
  char str[512];
  va_list args;
  va_start(args, format);
  r = vsprintf(str, format, args);
  va_end(args);
  return write(fd, str, r);
}
