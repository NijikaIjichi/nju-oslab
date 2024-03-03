#include "lib.h"

int sem_init(sem_t sem, int val) {
  if (pipe(sem) < 0) return -1;
  char ch = 0;
  for (int i = 0; i < val; ++i) {
    write(sem[1], &ch, 1);
  }
  return 0;
}

void sem_wait(sem_t sem) {
  char ch;
  read(sem[0], &ch, 1);
}

void sem_post(sem_t sem) {
  char ch = 0;
  write(sem[1], &ch, 1);
}

void sem_destroy(sem_t sem) {
  close(sem[1]);
  close(sem[0]);
}

int svar_init(svar_t *svar, void *ptr, uint32_t size) {
  if (pipe(svar->fd) < 0) return -1;
  svar->ptr = ptr;
  svar->size = size;
  return 0;
}

void svar_read(svar_t *svar) {
  read(svar->fd[0], svar->ptr, svar->size);
}

void svar_write(svar_t *svar) {
  write(svar->fd[1], svar->ptr, svar->size);
}

void svar_close(svar_t *svar) {
  close(svar->fd[1]);
  close(svar->fd[0]);
}
