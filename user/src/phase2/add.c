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
  char *sh1_argv[] = {"sh1", NULL};
  exec("sh1", sh1_argv);
  assert(0);
}