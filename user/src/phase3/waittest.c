#include "ulib.h"

#define N  1000

void waittest(void) {
  int n, pid;

  for (n = 0; n < N; n++) {
    pid = fork();
    if (pid < 0)
      break;
    if (pid == 0)
      exit(0);
  }

  if (n == N) {
    printf("fork claimed to work N times!\n");
    goto bad;
  }

  for (; n > 0; n--) {
    if (wait(0) < 0) {
      printf("wait stopped early\n");
      goto bad;
    }
  }

  if (wait(0) != -1) {
    printf("wait got too many\n");
    goto bad;
  }

  return;

  bad:
  printf("\nwaittest failed!\n");
  exit(1);
}

int main() {
  printf("waittest start.\n");
  for (int i = 0; i < 3; ++i) {
    waittest();
    printf(".");
  }
  printf("\nwaittest passed!\n");
  return 0;
}
