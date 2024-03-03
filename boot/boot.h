#include <stdint.h>
#include <elf.h>

#define SECTSIZE 512

static inline uint8_t inb(int port) {
  uint8_t data;
  asm volatile ("inb %1, %0" : "=a"(data) : "d"((uint16_t)port));
  return data;
}

static inline uint32_t inl(int port) {
  uint32_t data;
  asm volatile ("inl %1, %0" : "=a"(data) : "d"((uint16_t)port));
  return data;
}

static inline void outb(int port, uint8_t data) {
  asm volatile ("outb %%al, %%dx" : : "a"(data), "d"((uint16_t)port));
}

static inline void wait_disk() {
  while ((inb(0x1f7) & 0xc0) != 0x40);
}

static inline void read_disk(void *buf, int sect) {
  wait_disk();
  outb(0x1f2, 1);
  outb(0x1f3, sect);
  outb(0x1f4, sect >> 8);
  outb(0x1f5, sect >> 16);
  outb(0x1f6, (sect >> 24) | 0xE0);
  outb(0x1f7, 0x20);
  wait_disk();
  for (int i = 0; i < SECTSIZE / 4; i ++) {
    ((uint32_t *)buf)[i] = inl(0x1f0);
  }
}

static inline void copy_from_disk(void *buf, int nbytes, int disk_offset) {
  uint32_t cur  = (uint32_t)buf;
  uint32_t ed   = (uint32_t)buf + nbytes;
  uint32_t sect = (disk_offset / SECTSIZE);
  for(; cur < ed; cur += SECTSIZE, sect ++)
    read_disk((void *)cur, sect);
}

#define SERIAL_PORT 0x3F8

static inline int serial_idle() {
  return (inb(SERIAL_PORT + 5) & 0x20) != 0;
}

static inline void putchar(char ch) {
  while (!serial_idle()) ;
  outb(SERIAL_PORT, ch);
}

static inline void putstr(const char *str) {
  for (; *str; ++str) {
    putchar(*str);
  }
}

static inline void memcpy(void *__restrict dst, const void *__restrict src, uint32_t size) {
  char *_dst = dst, *fi = _dst + size;
  const char *_src = src;
  while (_dst < fi) *_dst++ = *_src++;
}

static inline void memset(void *dst, int c, uint32_t size) {
  char *_dst = dst, *fi = _dst + size;
  while (_dst < fi) *_dst++ = (char)c;
}
