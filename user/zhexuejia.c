#include "lib.h"

#define N 5

int pid[N], curr;
sem_t kuaizi[N];
svar_t curr_s;

void zhexuejia(int i) {
  while (1) {
    P(kuaizi[i]);
    svar_read(&curr_s);
    ++curr;
    svar_write(&curr_s);
    P(kuaizi[(i + 1) % N]);
    char ch = '0' + i;
    write(1, &ch, 1);
    svar_read(&curr_s);
    --curr;
    svar_write(&curr_s);
    V(kuaizi[i]);
    V(kuaizi[(i + 1) % N]);
  }
}

int main() {
  int p = 0;
  for (int i = 0; i < N; ++i) {
    if (sem_init(kuaizi[i], 1) < 0) goto bad;
  }
  if (svar_init(&curr_s, &curr, sizeof(curr)) < 0) goto bad;
  svar_write(&curr_s);
  for (; p < N; ++p) {
    pid[p] = fork();
    if (pid[p] < 0) goto bad;
    if (pid[p] == 0) zhexuejia(p);
  }
  for (;;) {
    sleep(1);
    svar_read(&curr_s);
    if (curr == N) {
      printf("\nDead Lock!\n");
      svar_write(&curr_s);
      sleep(64);
      for (int i = 0; i < N; ++i) {
        kill(pid[i]);
      }
      break;
    }
    svar_write(&curr_s);
  }
  for (int i = 0; i < N; ++i) wait(NULL);
  return 0;
bad:
  for (int i = 0; i < p; ++i) {
    kill(pid[i]);
  }
  printf("zhexuejia: sth bad.\n");
  return 1;
}
