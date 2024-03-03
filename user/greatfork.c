#include "lib.h"

int main(int argc, char *argv[]) {
  int n = 0;
  if (argc != 2) {
    printf("usage: greatfork [n]\n");
    exit(1);
  }
  n = strtol(argv[1], NULL, 10);
  for (int i = 0; i < n; ++i) fork();
  for (int i = 0; i < n; ++i) wait(NULL);
  return 0;
}
