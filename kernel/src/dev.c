#include "klib.h"
#include "dev.h"
#include "serial.h"
#include "fs.h"

static int ban_read(void *buf, size_t count) {
  return -1;
}

static int ignore_write(const void *buf, size_t count) {
  return count;
}

static struct {
  char name[32];
  dev_t dev_op;
} dev_table[] = {
  {"/dev/serial", {serial_read, serial_write}},
  {"/dev/null", {ban_read, ignore_write}}
};

#define DEV_NUM (sizeof(dev_table) / sizeof(dev_table[0]))

void init_dev() {
  iclose(iopen("/dev", TYPE_DIR)); // create the dev dir
  for (int i = 0; i < DEV_NUM; ++i) {
    iadddev(dev_table[i].name, i);
  }
}

dev_t *dev_get(int id) {
  if (id >= DEV_NUM) return NULL;
  return &dev_table[id].dev_op;
}
