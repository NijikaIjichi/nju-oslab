#include "boot.h"
#include <elf.h>

#define SECTSIZE 512

void memcpy(void *dst, const void *src, uint32_t size) {
  char *_dst = dst, *fi = _dst + size;
  const char *_src = src;
  while (_dst < fi) *_dst++ = *_src++;
}

void memset(void *dst, int c, uint32_t size) {
  char *_dst = dst, *fi = _dst + size;
  while (_dst < fi) *_dst++ = (char)c;
}

void bootMain(void) {
  Elf32_Ehdr *elf = (void*)0x200000;
  Elf32_Phdr *ph, *eph;
  uint32_t entry;
  for (int i = 0; i < 255; ++i)
    readSect((void*)((uint32_t)elf + i * 512), i + 1);
  entry = elf->e_entry;
  ph = (void*)((uint32_t)elf + elf->e_phoff);
  eph = ph + elf->e_phnum;
  for (; ph < eph; ph++) {
    if (ph->p_type == PT_LOAD) {
      uint32_t va = ph->p_vaddr;
      memcpy((void*)va, (void*)((uint32_t)elf + ph->p_offset), ph->p_filesz);
      memset((void*)(va + ph->p_filesz), 0, ph->p_memsz - ph->p_filesz);
    }
  }
  ((void(*)())entry)();
}

void waitDisk(void) { // waiting for disk
  while((inByte(0x1F7) & 0xC0) != 0x40);
}

void readSect(void *dst, int offset) { // reading a sector of disk
  int i;
  waitDisk();
  outByte(0x1F2, 1);
  outByte(0x1F3, offset);
  outByte(0x1F4, offset >> 8);
  outByte(0x1F5, offset >> 16);
  outByte(0x1F6, (offset >> 24) | 0xE0);
  outByte(0x1F7, 0x20);

  waitDisk();
  for (i = 0; i < SECTSIZE / 4; i ++) {
    ((int *)dst)[i] = inLong(0x1F0);
  }
}
