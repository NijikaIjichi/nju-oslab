#include "ulib.h"

#define M 998244353
#define ANS 289377997 // 51199840000 % M

void child(size_t i) {
  // add [i*10000, (i+1)*10000)
  size_t ans = 0;
  for (size_t j = i*10000; j < (i+1)*10000; ++j) {
    ans = (ans + j) % M;
  }
  exit((int)ans);
}

int main() {
  size_t ans = 0;
  printf("multiadd start.\n");
  for (size_t i = 0; i < 32; ++i) {
    if (fork() == 0) child(i);
  }
  for (int i = 0; i < 32; ++i) {
    int exitcode;
    wait(&exitcode);
    ans = (ans + (size_t)exitcode) % M;
  }
  printf("ans = %u.\n", ans);
  if (ans == ANS) {
    printf("multiadd passed!\n");
  } else {
    printf("multiadd failed!\n");
  }
  return ans != ANS;
}
