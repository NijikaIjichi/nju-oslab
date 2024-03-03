#include "lib.h"

uint32_t syscall(int num, uint32_t args[]) {
  uint32_t ret = 0;
  asm volatile("int $0x80" : "=a"(ret) : "a"(num), "c"(args[0]), "d"(args[1]), "b"(args[2]), "S"(args[3]), "D"(args[4]));
  return ret;
}

