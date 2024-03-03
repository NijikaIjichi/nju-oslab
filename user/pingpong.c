#include "lib.h"

int main(int argc, char *argv[]) {
  int data = 0, i = 8, pid, x = 9;
  int p2c[2], c2p[2];
  if (argc > 1) {
    i = strtol(argv[1], NULL, 10);
    x = i + 1;
  }
  if (pipe(p2c) < 0 || pipe(c2p) < 0) {
    printf("pingpong: pipe failed.\n");
    return 1;    
  }
  pid = fork();
  if (pid < 0) {
    printf("pingpong: fork failed.\n");
    return 1;
  } else if (pid == 0) {
    close(p2c[1]);
    close(c2p[0]);
    data = 2;
    srand(2);
    for (; i > 0; i--) {
      read(p2c[0], &x, sizeof(x));
      printf("child: %d - %d - %d.\n", i, x, data);
      write(c2p[1], &i, sizeof(i));
      sleep(rand() % 128);
    }
  } else {
    close(p2c[0]);
    close(c2p[1]);
    data = 1;
    srand(1);
    for (; i > 0; i--) {
      printf("parent: %d - %d - %d.\n", i, x, data);
      write(p2c[1], &i, sizeof(i));
      read(c2p[0], &x, sizeof(x));
      sleep(rand() % 128);
    }
    wait(&x);
    printf("child exit @ %d.\n", x);
  }
  return 0;
}
