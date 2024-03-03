#ifndef __FILE_H__
#define __FILE_H__

#include <stdint.h>
#include "fs.h"
#include "dev.h"

typedef struct file {
  int type;
  int ref;
  int readable, writable;

  // for normal file
  inode_t *inode;
  uint32_t offset;

  // for dev file
  dev_t *dev_op;
} file_t;

file_t *fopen(const char *path, int mode);
int fread(file_t *file, void *buf, uint32_t size);
int fwrite(file_t *file, const void *buf, uint32_t size);
uint32_t fseek(file_t *file, uint32_t off, int whence);
file_t *fdup(file_t *file);
void fclose(file_t *file);

#endif
