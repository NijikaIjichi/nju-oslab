#include "lib.h"

static unsigned long long next = 1;

void srand(unsigned int seed) {
  next = seed;
}

int rand() {
  next = next * 6364136223846793005LL + 1;
  return (int)((next >> 32) & RAND_MAX);
}
