#include "ulib.h"

#define MAX_ARG_LEN  31
#define MAX_ARGS_NUM 31
#define ARGS_BUF_LEN ((MAX_ARGS_NUM+1) * (MAX_ARG_LEN+1))

char args[ARGS_BUF_LEN];

int main() {
  int argc;
  char *argv[MAX_ARGS_NUM+1];
  for (;;) {
    printf("$ ");
    getline(args, ARGS_BUF_LEN);
    argc = 0;
    for (char *c = strtok(args, " \t\n"); c; c = strtok(NULL, " \t\n")) {
      if (argc == MAX_ARGS_NUM) break;
      argv[argc++] = c;
    }
    if (argc > 0) {
      argv[argc] = NULL;
      if (exec(argv[0], argv) == -1) {
        printf("sh: exec failed.\n");
      }
    }
  }
}
