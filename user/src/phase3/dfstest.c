#include "ulib.h"

void dfs_test();

int main() {
  dfs_test();
  return 0;
}

#define DEST  '+'
#define EMPTY '.'

static struct move {
  int x, y, ch;
} moves[] = {
  { 0, 1, '>' },
  { 1, 0, 'v' },
  { 0, -1, '<' },
  { -1, 0, '^' },
};

static char map[][16] = {
  "#####",
  "#..+#",
  "##..#",
  "#####",
  "",
};

void display();

void dfs(int x, int y) {
  if (map[x][y] == DEST) {
    display();
  } else {
    sleep(10);
    int nfork = 0;

    for (struct move *m = moves; m < moves + 4; m++) {
      int x1 = x + m->x, y1 = y + m->y;
      if (map[x1][y1] == DEST || map[x1][y1] == EMPTY) {
        int pid = fork();
        if (pid == 0) { // map[][] copied
          map[x][y] = m->ch;
          dfs(x1, y1);
          exit(0);
        } else {
          nfork++;
        }
      }
    }
    while (nfork--) wait(NULL);
  }
}

void dfs_test() {
  dfs(1, 1);
}

void display() {
  char buf[128] = "";
  for (int i = 0; ; i++) {
    strcat(buf, map[i]);
    strcat(buf, "\n");
    if (strlen(map[i]) == 0) break;
  }
  printf("%s", buf);
}
