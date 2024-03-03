#ifndef __DEV_H__
#define __DEV_H__

#include <stdint.h>

typedef struct dev {
  int (*read)(void *buf, uint32_t size);
  int (*write)(const void *buf, uint32_t size);
} dev_t;

void init_dev();
dev_t *dev_get(int id);

#endif
