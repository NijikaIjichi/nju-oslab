#include "x86.h"
#include "device.h"
#include "sys.h"
#include "proc.h"
#include "fs.h"

uint32_t tty_read(char *buf, uint32_t size);
uint32_t tty_write(const char *buf, uint32_t size);
uint32_t ban_read(char *buf, uint32_t size);
uint32_t serial_write(const char *buf, uint32_t size);
uint32_t null_write(const char *buf, uint32_t size);

device_t device_table[] = {
  {tty_read, tty_write},
  {ban_read, serial_write},
  {ban_read, null_write},
};

const char *device_name[] = {
  "/dev/tty",
  "/dev/serial",
  "/dev/null",
  NULL,
};

uint32_t tty_read(char *buf, uint32_t size) {
  char ch = 0;
  uint32_t i = 0;
  for (; i < size && ch != '\n'; ++i) {
    push_off();
    while ((ch = key_buf_pop_front()) == 0) {
      sleep(&key_buf_lock);
    }
    pop_off();
    buf[i] = ch;
  }
  return i;
}

uint32_t tty_write(const char *buf, uint32_t size) {
  push_off();
  for (uint32_t i = 0; i < size; ++i) {
    putch(buf[i]);
  }
  pop_off();
  return size;
}

uint32_t ban_read(char *buf, uint32_t size) {
  return 0;
}

uint32_t serial_write(const char *buf, uint32_t size) {
  push_off();
  for (uint32_t i = 0; i < size; ++i) {
    putChar(buf[i]);
  }
  pop_off();
  return size;
}

uint32_t null_write(const char *buf, uint32_t size) {
  return size;
}

device_t *device_lookup(int device_id) {
  if (device_id < 0 || device_id >= LENGTH(device_table)) return NULL;
  return &device_table[device_id];
}

void init_device() {
  iput(iopen("/dev", NULL, 1, TYPE_DIR, 0, NULL));
  for (int i = 0; device_name[i]; ++i) {
    iput(iopen(device_name[i], NULL, 1, TYPE_DEV, i, NULL));
  }
}
