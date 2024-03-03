#include "lib.h"

int main(int argc, char *argv[]) {
  int s = 64;
  if (argc > 1) s = atoi(argv[1]);
  int pid = fork();
  if (pid < 0) printf("killchild: fork failed.\n");
  if (pid == 0) for (;;) sleep(1);
  else {
    if (s > 0) sleep(s);
    if (kill(pid) < 0) {printf("killchild: kill failed.\n"); exit(-1);};
    int ppid, status;
    ppid = wait(&status);
    if (pid != ppid) {printf("killchild: wrong pid %d.\n", ppid); exit(-1);}
    if (status != -1) {printf("killchild: wrong status %d.\n", status); exit(-1);}
  }
  return 0;
}
