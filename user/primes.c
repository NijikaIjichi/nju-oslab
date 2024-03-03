#include "lib.h"

#define stdout 1
#define stderr 2
#define READ 0
#define WRITE 1
#define PARENT (curr_p % 2)
#define CHILD (1 - curr_p % 2)

static int pipe_p2c[2][2] = {0};
static int curr_p = 0;

static inline void safe_pipe(int* p) {
  if (pipe(p) == -1) {
    fprintf(stderr, "primes: open pipe failed.\n");
    exit(1);
  }
}

static inline int safe_fork() {
  int pid;
  if ((pid = fork()) == -1) {
    fprintf(stderr, "primes: fork failed.\n");
    exit(1);
  }
  return pid;
}

void child_process();

int main(int argc, char *argv[]) {
  int n = 36;
  if (argc > 1) {
    n = strtol(argv[1], NULL, 10);
  }
  safe_pipe(pipe_p2c[0]);
  if (safe_fork() == 0) {
    child_process();
  } else {
    close(pipe_p2c[0][READ]);
    int i;
    for (i = 2; i < n; ++i) {
      write(pipe_p2c[0][WRITE], &i, 4);
    }
    close(pipe_p2c[0][WRITE]);
  }
  wait(NULL);
  return 0;
}

void child_process() {
  ++curr_p;
  close(pipe_p2c[CHILD][WRITE]);
  if (pipe_p2c[PARENT][READ]) {
    close(pipe_p2c[PARENT][READ]);
  }
  int prime;
  if (read(pipe_p2c[CHILD][READ], &prime, 4) == 0) {
    close(pipe_p2c[CHILD][READ]);
    exit(0);
  }
  fprintf(stdout, "prime %d\n", prime);
  safe_pipe(pipe_p2c[PARENT]);
  if (safe_fork() == 0) {
    child_process();
  } else {
    close(pipe_p2c[PARENT][READ]);
    int num;
    while (read(pipe_p2c[CHILD][READ], &num, 4) != 0) {
      if (num % prime != 0) {
        write(pipe_p2c[PARENT][WRITE], &num, 4);
      }
    }
    close(pipe_p2c[PARENT][WRITE]);
    close(pipe_p2c[CHILD][READ]);
    wait(NULL);
  }
}
