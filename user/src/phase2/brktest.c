#include "ulib.h"

void test() {
  printf("brktest: start\n");
  volatile uint8_t *m = sbrk(4096 * 10);
  assert(m != (void*)-1);
  for (int i = 0; i < 10; ++i) {
    m[i * 4096 + i] = (i & 0xff);
  }
  for (int i = 0; i < 10; ++i) {
    assert(m[i * 4096 + i] == (i & 0xff));
  }
  sbrk(-4096 * 10);
  for (int i = 0; i < 10; ++i) {
    volatile uint8_t *n = malloc(4096);
    assert(n);
    n[i] = (i & 0xff);
    assert(n[i] == (i & 0xff));
    free((void*)n);
  }
  volatile uint8_t *ptrs[10];
  for (int i = 0; i < 10; ++i) {
    volatile uint8_t *n = ptrs[i] = malloc(4096);
    assert(n);
    n[i] = (i & 0xff);
    assert(n[i] == (i & 0xff));
  }
  for (int i = 0; i < 10; ++i) {
    assert(ptrs[i][i] == (i & 0xff));
    free((void*)ptrs[i]);
  }
  printf("brktest: passed!\n");
}

int main() {
  test();
  while (1) ;
}
