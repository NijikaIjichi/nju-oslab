#include "ulib.h"

#define BUF_SIZE 32
#define PRODUCER_NUM 4
#define COMSUMER_NUM 2

int empty, full, mutex;

int fork_s() {
  int pid = fork();
  assert(pid >= 0);
  return pid;
}

void produce_one() {
  sleep(rand() % 5 + 10);
}

void consume_one() {
  sleep(rand() % 10 + 5);
}

void producer(int id) {
  while (1) {
    produce_one();
    P(empty);
    P(mutex);
    // put to buffer
    printf("producer %d: produce\n", id);
    V(mutex);
    V(full);
  }
}

void consumer(int id) {
  while (1) {
    P(full);
    P(mutex);
    // take from buffer
    printf("consumer %d: consume\n", id);
    V(mutex);
    V(empty);
    consume_one();
  }
}

int main() {
  empty = sem_open(BUF_SIZE);
  full = sem_open(0);
  mutex = sem_open(1);
  assert(empty >= 0 && full >= 0 && mutex >= 0);
  printf("producer-consumer start\n");
  for (int i = 0; i < PRODUCER_NUM; ++i) {
    if (fork_s() == 0) {
      srand(i + 1);
      producer(i);
    }
  }
  for (int i = 0; i < COMSUMER_NUM; ++i) {
    if (fork_s() == 0) {
      srand(~i);
      consumer(i);
    }
  }
  while (1) ;
}
