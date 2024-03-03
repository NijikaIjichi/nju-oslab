#ifndef __DISK_H__
#define __DISK_H__

#include <stdint.h>

#define SECTSIZE 512

void read_disk(void *buf, int sect);
void write_disk(const void *buf, int sect);
void copy_from_disk(void *buf, int nbytes, int disk_offset);
void copy_to_disk(const void *buf, int nbytes, int disk_offset);

#define BLK_SIZE (SECTSIZE * 8)

void bread(void *dst, uint32_t size, uint32_t no, uint32_t off);
void bwrite(const void *src, uint32_t size, uint32_t no, uint32_t off);
void bzero(uint32_t no);

#endif
