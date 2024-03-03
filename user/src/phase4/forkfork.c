#include "ulib.h"

void
forkfork()
{
  enum { N=2 };
  
  for(int i = 0; i < N; i++){
    int pid = fork();
    if(pid < 0){
      printf("forkfork: fork failed\n");
      exit(1);
    }
    if(pid == 0){
      for(int j = 0; j < 100; j++){
        int pid1 = fork();
        if(pid1 < 0){
          exit(1);
        }
        if(pid1 == 0){
          exit(0);
        }
        wait(0);
      }
      exit(0);
    }
  }

  int xstatus;
  for(int i = 0; i < N; i++){
    wait(&xstatus);
    if(xstatus != 0) {
      printf("forkfork: fork in child failed\n");
      exit(1);
    }
  }
}

int main() {
  printf("forkfork start\n");
  int pid = fork();
  assert(pid >= 0);
  if (pid == 0) {
    forkfork();
    exit(0);
  }
  int status;
  wait(&status);
  if (status == 0) {
    printf("forkfork passed!\n");
  } else {
    printf("forkfork failed!\n");
  }
  return 0;
}
