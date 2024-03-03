#include "ulib.h"

void test() {
  printf("sleeptest start.\n");
  sleep(100);
  printf("sleeptest passed!\n");
}

int main() {
  test();
  while (1) ;
}
