#ifndef __SEM_H__
#define __SEM_H__

#include "klib.h"

typedef struct sem {
  int value;
  list_t wait_list;
} sem_t;

void sem_init(sem_t *sem, int value);
void sem_p(sem_t *sem);
void sem_v(sem_t *sem);

typedef struct usem {
  sem_t sem;
  int ref;
} usem_t;

usem_t *usem_alloc(int value);
usem_t *usem_dup(usem_t *usem);
void usem_close(usem_t *usem);

#endif
