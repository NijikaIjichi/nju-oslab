#include "lib.h"

#define P_NUM 4
#define BIAS 20
#define N 64
#define SUM ((((P_NUM-1)<<BIAS)*P_NUM/2)*N+((N-1)*N/2)*P_NUM)

int pip[2];

void p(int id) {
  for (int i = 0; i < N; ++i) {
    int c = (id << BIAS) + i;
    printf("Producer %d: produce %d\n", id, i);
    sleep(16);
    write(pip[1], &c, sizeof(c));
    sleep(16);
  }
  exit(0);
}

int main() {
  pipe(pip);
  for (int i = 0; i < P_NUM; ++i) {
    if (fork() == 0) p(i);
  }
  int c = 0, sum = 0;
  close(pip[1]);
  while (read(pip[0], &c, sizeof(c)) > 0) {
    printf("Consumer: consume\n");
    sum += c;
  }
  if (sum == SUM) {
    printf("Accepted, sum = %d.\n", sum);
  } else {
    printf("Rejected, sum = %d, expect = %d.\n", sum, SUM);
  }
  return 0;
}
