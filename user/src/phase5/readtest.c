#include "ulib.h"

char *ans[] = {"hello world\n", "nihao shijie\n", "ohayou seikai\n"};
int fd, sems[3];
char buf[512];

int fork_s() {
  int pid = fork();
  assert(pid >= 0);
  return pid;
}

void test(int i) {
  if (i > 0) sem_p(sems[i-1]);
  if (i < 3) {
    int l = strlen(ans[i]);
    assert(read(fd, buf, l) == l);
    assert(strcmp(buf, ans[i]) == 0);
    sem_v(sems[i]);
  } else {
    assert(read(fd, buf, sizeof buf) == 0);
    assert(lseek(fd, 0, SEEK_CUR) == 39);
    assert(lseek(fd, 12, SEEK_SET) == 12);
    int l = strlen(ans[1]), fd2 = dup(fd);
    assert(fd2 >= 0);
    assert(read(fd2, buf, l) == l);
    assert(strcmp(buf, ans[1]) == 0);
    assert(lseek(fd, -14, SEEK_END) == 25);
    l = strlen(ans[2]);
    assert(read(fd2, buf, l) == l);
    assert(strcmp(buf, ans[2]) == 0);
  }
  exit(0);
}

int main() {
  for (int i = 0; i < 3; ++i) {
    sems[i] = sem_open(0);
    assert(sems[i] >= 0);
  }
  fd = open("words.txt", O_RDONLY);
  assert(fd >= 0);
  printf("readtest start\n");
  for (int i = 0; i < 4; ++i) {
    if (fork_s() == 0) test(i);
  }
  for (int i = 0; i < 4; ++i) {
    int status = -1;
    wait(&status);
    assert(status == 0);
  }
  printf("readtest passed!\n");
  return 0;
}
