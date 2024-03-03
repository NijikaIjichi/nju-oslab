#ifndef __DEVICE_H__
#define __DEVICE_H__

#include "device/serial.h"
#include "device/disk.h"
#include "device/vga.h"
#include "device/keyboard.h"

typedef struct device {
  uint32_t (*read)(char *buf, uint32_t size);
  uint32_t (*write)(const char *buf, uint32_t size);
} device_t;

device_t *device_lookup(int device_id);
void init_device();

#endif
