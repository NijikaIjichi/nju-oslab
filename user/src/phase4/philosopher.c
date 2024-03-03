#include "philosopher.h"

// TODO: define some sem if you need
int chops[PHI_NUM];
int bowl;

void init() {
  // TODO: init some sem if you need
  for (int i = 0; i < PHI_NUM; ++i) {
    chops[i] = sem_open(1);
    assert(chops[i] >= 0);
  }
  bowl = sem_open(PHI_NUM - 1);
  assert(bowl >= 0);
}

void philosopher(int id) {
  // TODO: implement philosopher, remember to call `eat` and `think`
  while (1) {
    P(bowl);
    P(chops[id]);
    P(chops[(id + 1) % PHI_NUM]);
    eat(id);
    V(chops[(id + 1) % PHI_NUM]);
    V(chops[id]);
    V(bowl);
    think(id);
  }
}
