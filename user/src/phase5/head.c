#include "ulib.h"

char buf[4096];

int write_s(int fd, const void *buf, size_t count) {
  if (write(fd, buf, count) != count) {
    fprintf(2, "head: write error\n");
    exit(1);
  }
  return count;
}

void head(int fd) {
  int n;
  int line = 0;
  while (line < 10 && (n = read(fd, buf, sizeof(buf))) > 0) {
    char *p = buf;
    while (line < 10) {
      char *e = strchr(p, '\n');
      if (e) {
        ++line;
        write_s(1, p, e - p + 1);
        p = e + 1;
      } else {
        write_s(1, p, strlen(p));
        break;
      }
    }
  }
  if (n < 0) {
    fprintf(2, "head: read error\n");
    exit(1);
  }
}

int main(int argc, char *argv[]) {
  int fd, i;
  if (argc <= 1) {
    head(0);
    exit(0);
  }
  for (i = 1; i < argc; i++) {
    if ((fd = open(argv[i], 0)) < 0) {
      fprintf(2, "head: cannot open %s\n", argv[i]);
      exit(1);
    }
    head(fd);
    close(fd);
  }
  exit(0);
}
