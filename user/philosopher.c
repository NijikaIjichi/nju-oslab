#include "lib.h"

#define N 5
sem_t kuaizi[N], total;

void phi(int id) {
  while (1) {
    printf("Philosopher %d: think\n", id);
    sleep(16);
    P(total);
    P(kuaizi[id]);
    P(kuaizi[(id + 1) % N]);
    printf("Philosopher %d: eat\n", id);
    sleep(16);
    V(kuaizi[id]);
    V(kuaizi[(id + 1) % N]);
    V(total);
  }
}

int main() {
  for (int i = 0; i < N; ++i) {
    sem_init(kuaizi[i], 1);
  }
  sem_init(total, N - 1);
  for (int i = 1; i < N; ++i) {
    if (fork() == 0) phi(i);
  }
  phi(0);
  return 0;
}
