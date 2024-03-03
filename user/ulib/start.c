#include "ulib.h"

int main(int argc, char *argv[]);

void _start(int argc, char *argv[]) {
  exit(main(argc, argv));
}

int abort(const char *file, int line, const char *info) {
  printf("Abort @ [%s:%d] %s\n", file, line, info ? info : "");
  //exit(1); // uncomment me at Lab2-3
  while (1) ;
}
