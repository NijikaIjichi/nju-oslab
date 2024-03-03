#include "lib.h"

#define R_N 3
#define W_N 3

sem_t write_mutex;
int rcount;
svar_t rcount_s;

void writer(int id) {
  while (1) {
    P(write_mutex);
    printf("Writer %d: write\n", id);
    sleep(16);
    V(write_mutex);
    sleep(16);
  }
}

void reader(int id) {
  while (1) {
    svar_read(&rcount_s);
    if (rcount == 0) P(write_mutex);
    ++rcount;
    svar_write(&rcount_s);
    printf("Reader %d: read, total %d reader\n", id, rcount);
    sleep(16);
    svar_read(&rcount_s);
    --rcount;
    if (rcount == 0) V(write_mutex);
    svar_write(&rcount_s);
    sleep(16);
  }
}

int main() {
  sem_init(write_mutex, 1);
  svar_init(&rcount_s, &rcount, sizeof(rcount));
  svar_write(&rcount_s);
  for (int i = 0; i < W_N; ++i) {
    if (fork() == 0) writer(i);
  }
  for (int i = 0; i < R_N; ++i) {
    if (fork() == 0) reader(i);
  }
  wait(NULL);
  return 0;
}
