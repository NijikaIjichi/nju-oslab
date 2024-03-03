#include "ulib.h"

const char rodata_buf[4500] = {[0] = 'o', 's', 'l', 'a', 'b', '-', '1',
                               [4300] = 'O', 'S', 'L', 'A', 'B', '-', '1'};
char data_buf[4500] = {[0] = 'a', 'b', 'c', 'd', 'e', 'f', 'g',
                       [4200] = 'A', 'B', 'C', 'D', 'E', 'F', 'G'};
char bss_buf[4500];

void test() {
  printf("loaduser test: start\n");
  assert(strcmp(rodata_buf, "oslab-1") == 0);
  assert(strcmp(&rodata_buf[4300], "OSLAB-1") == 0);
  assert(strcmp(data_buf, "abcdefg") == 0);
  assert(strcmp(&data_buf[4200], "ABCDEFG") == 0);
  for (int i = 0; i < 4500 / 4; ++i) {
    assert(((int*)bss_buf)[i] == 0);
  }
  printf("loaduser test: passed!\n");
}

int main() {
  printf("Hello, I am at 0x%p\n", main);
  test();
  while (1) ;
}
