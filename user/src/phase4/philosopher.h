#include "ulib.h"

#define PHI_NUM 5

void init();
void philosopher(int id);

void eat(int id) {
  printf("philosopher %d start eating.\n", id);
  sleep(rand() % 10 + 5);
  printf("philosopher %d end eating.\n", id);
}

void think(int id) {
  //printf("philosopher %d start thinking.\n", id);
  sleep(rand() % 5 + 10);
  //printf("philosopher %d end thinking.\n", id);
}

int fork_s() {
  int pid = fork();
  assert(pid >= 0);
  return pid;
}

int main() {
  init();
  printf("philosopher start.\n");
  for (int i = 0; i < PHI_NUM; ++i) {
    if (fork_s() == 0) {
      srand(i + 1);
      philosopher(i);
    }
  }
  while (1) ;
}
