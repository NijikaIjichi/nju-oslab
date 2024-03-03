#include "lib.h"

int main() {
  int pid, wpid, status;
  char *argv[] = {"bash", NULL};
  open("/dev/tty", O_RDWR);
  dup(0);
  open("/dev/serial", O_WRONLY);
  for (;;) {
    pid = fork();
    if (pid < 0) {
      printf("init: fork failed.\n");
      return 1;
    } else if (pid == 0) {
      exec("bash", argv);
      printf("init: start bash failed.\n");
      return 1;
    }
    //fprintf(STD_ERR, "pid = %d is the bash\n", pid);
    for (;;) {
      wpid = wait(&status);
      //fprintf(STD_ERR, "pid = %d exit @ %d, is it a zombie?\n", wpid, status);
      if (pid == wpid) {
        //fprintf(STD_ERR, "wait, bash exit? how does it happened?\n");
        break;
      }
    }
  }
}
