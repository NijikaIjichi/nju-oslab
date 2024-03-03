#include "ulib.h"

int sem_id;

void ping(int pid, int x) {
  srand(x == 0 ? 1 : x);
  for (int i = 0; i < 8; ++i) {
    printf("ping: pid=%d, i=%d, x=%d\n", pid, i, x);
    V(sem_id);
    sleep(rand() % 20 + 20);
  }
}

void pong(int pid, int x) {
  srand(x == 0 ? 1 : x);
  for (int i = 0; i < 8; ++i) {
    P(sem_id);
    printf("pong: pid=%d, i=%d, x=%d\n", pid, i, x);
    sleep(rand() % 20 + 20);
  }
}

int main(int argc, char *argv[]) {
  int x = argc > 1 ? atoi(argv[1]) : 0;
  int y = argc > 2 ? atoi(argv[2]) : 2;
  printf("pingpong start\n");
  sem_id = sem_open(0);
  if (sem_id < 0) {
    printf("pingpong: sem open failed\n");
    return 1;
  }
  int pid = fork();
  if (pid == -1) {
    printf("pingpong: fork failed\n");
    return 1;
  } else if (pid != 0) { // parent
    ping(getpid(), x);
    assert(wait(NULL) == pid);
  } else { // child
    pong(getpid(), y);
  }
  return 0;
}
