#include "ulib.h"
#include "sysnum.h"

void test() {
  printf("systest: start\n");
  volatile int local = 0x01020304;
  volatile int ret = 0;
  asm volatile (
    "pushl %%ebp;"
    "movl %%esp, %%ebp;"
    "movl $0xfffffffc, %%esp;" // bug esp, make sure stack change
    "int $0x80;"
    "movl %%ebp, %%esp;"
    "popl %%ebp;"
    : "=a"(ret)
    : "a"(SYS_write), "b"(1), "c"("systest: "), "d"(9)
  );
  if (ret == 9 && local == 0x01020304) {
    printf("passed!\n");
  } else {
    printf("failed!\n");
  }
}

int main() {
  test();
  while (1) ;
}
