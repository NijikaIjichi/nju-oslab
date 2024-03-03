#include "ulib.h"

int main(int argc, char *argv[]) {
  if (argc != 4) goto bad;
  int mode = strtol(argv[1], NULL, 10);
  int pticks = strtol(argv[2], NULL, 10);
  int cticks = strtol(argv[3], NULL, 10);
  int pid = fork();
  if (pid == -1) {
    printf("childtest: fork failed.\n");
    return 1;
  } else if (pid == 0) {
    if (cticks > 0) {
      printf("child sleep...\n");
      sleep(cticks);
    }
    exit(0);
  } else {
    int status;
    if (pticks > 0) {
      printf("parent sleep...\n");
      sleep(pticks);
    }
    if (mode) {
      assert(wait(&status) == pid);
      printf("child exit @ %d\n", status);
    }
    return 0;
  }
bad:
  printf("usage: childtest [wait or not(0/1)] [parent sleep ticks] [child sleep ticks]\n");
  return 1;
}
