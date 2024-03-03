#include "ulib.h"

int main(int argc, char *argv[]) {
  if (argc < 2) {
    printf("add: missing operand\n");
  }
  int total = 0;
  for (int i = 1; i < argc; ++i) {
    total += atoi(argv[i]);
  }
  printf("%d\n", total);
  return 0;
}