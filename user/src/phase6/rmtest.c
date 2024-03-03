#include "ulib.h"

int sem1, sem2;

#define BUFSIZE 512

char buf1[BUFSIZE], buf2[BUFSIZE], buf3[BUFSIZE];

void init() {
  unlink("test");
  int fd = open("test", O_WRONLY | O_TRUNC | O_CREATE);
  assert(fd >= 0);
  memset(buf1, 'a', BUFSIZE);
  memset(buf2, 'a', BUFSIZE);
  memset(buf3, 'b', BUFSIZE);
  for (int i = 0; i < 512; ++i) {
    assert(write(fd, buf1, BUFSIZE) == BUFSIZE);
  }
  close(fd);
  sem1 = sem_open(0);
  sem2 = sem_open(0);
  assert(sem1 >= 0 && sem2 >= 0);
}

void test1() {
  int fd = open("test", O_RDONLY);
  assert(fd >= 0);
  for (int i = 0; i < 256; ++i) {
    assert(read(fd, buf1, BUFSIZE) == BUFSIZE);
    assert(memcmp(buf1, buf2, BUFSIZE) == 0);
  }
  sem_v(sem1);
  sem_p(sem2);
  for (int i = 0; i < 256; ++i) {
    assert(read(fd, buf1, BUFSIZE) == BUFSIZE);
    assert(memcmp(buf1, buf2, BUFSIZE) == 0);
  }
  assert(read(fd, buf1, BUFSIZE) == 0);
  exit(0);
}

void test2() {
  sem_p(sem1);
  assert(unlink("test") == 0);
  sem_v(sem2);
  sem_v(sem2);
  int fd = open("test", O_WRONLY | O_TRUNC | O_CREATE);
  assert(fd >= 0);
  assert(write(fd, buf3, BUFSIZE) == BUFSIZE);
  close(fd);
  sem_v(sem2);
  sem_v(sem2);
}

int fork_s() {
  int pid = fork();
  assert(pid >= 0);
  return pid;
}

int main() {
  init();
  printf("rmtest start\n");
  for (int i = 0; i < 4; ++i) {
    if (fork_s() == 0)
      test1();
  }
  test2();
  for (int i = 0; i < 4; ++i) {
    int status;
    wait(&status);
    assert(status == 0);
  }
  printf("rmtest passed\n");
  return 0;
}
