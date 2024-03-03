#include "ulib.h"

void test() {
  printf("iotest start.\n");
  printf("input two numbers: $ ");
  int a, b;
  scanf("%d%d", &a, &b);
  printf("%d + %d = %d\n", a, b, a + b);
  printf("iotest passed!\n");
}

int main() {
  test();
  while (1) ;
}
