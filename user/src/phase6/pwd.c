#include "ulib.h"

char buf[512], *p = &buf[510];

void pwd() {
  while (1) {
    int fd = open(".", O_RDONLY), pfd = open("..", O_RDONLY);
    assert(fd >= 0 && pfd >= 0);
    struct stat st;
    assert(fstat(fd, &st) >= 0);
    int node = st.node;
    assert(fstat(pfd, &st) >= 0);
    if (node == st.node) {
      *p = '/';
      break;
    }
    struct dirent di;
    while (read(pfd, &di, sizeof di) > 0) {
      if (di.node == node) {
        p -= strlen(di.name) + 1;
        assert(p >= buf);
        memcpy(p + 1, di.name, strlen(di.name));
        *p = '/';
        goto found;
      }
    }
    assert(0);
    found:
    close(fd);
    close(pfd);
    chdir("..");
  }
}

int main() {
  pwd();
  printf("%s\n", p);
  return 0;
}
