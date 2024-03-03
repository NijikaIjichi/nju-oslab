#include "klib.h"

int abort(const char *file, int line, const char *info) {
  cli();
  printf("Abort @ [%s:%d] %s\n", file, line, info ? info : "");
  while (1) hlt();
}
