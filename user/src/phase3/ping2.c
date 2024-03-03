#include "ulib.h"

int main(int argc, char *argv[]) {
  int pid = getpid();
  int x = argc > 1 ? atoi(argv[1]) : 0;
  for (int i = 0; i < 8; ++i) {
    printf("ping: pid=%d, i=%d, x=%d\n", pid, i, x);
    sleep(25);
  }
  while (1) ;
}
