#include "ulib.h"

int x;
char buf[4096];

void test() {
  printf("pgfault test: start\n");
  x = ((volatile int *)buf)[4096]; // expect pagefault
  printf("pgfault test: failed!\n");
}

int main() {
  test();
  while (1) ;
}