#include "ulib.h"

int main(int argc, char *argv[]) {
  if (argc < 2) {
    printf("sleep: missing operand\n");
  }
  for (int i = 1; i < argc; ++i) {
    sleep(atoi(argv[i]));
  }
  return 0;
}
