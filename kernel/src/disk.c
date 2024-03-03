#include "klib.h"
#include "disk.h"

static inline void wait_disk() {
  while ((inb(0x1f7) & 0xc0) != 0x40);
}

void read_disk(void *buf, int sect) {
  wait_disk();
  outb(0x1f2, 1);
  outb(0x1f3, sect);
  outb(0x1f4, sect >> 8);
  outb(0x1f5, sect >> 16);
  outb(0x1f6, (sect >> 24) | 0xE0);
  outb(0x1f7, 0x20);
  wait_disk();
  for (int i = 0; i < SECTSIZE / 4; i ++) {
    ((uint32_t *)buf)[i] = inl(0x1f0);
  }
}

void write_disk(const void *buf, int sect) {
  wait_disk();
  outb(0x1f2, 1);
  outb(0x1f3, sect);
  outb(0x1f4, sect >> 8);
  outb(0x1f5, sect >> 16);
  outb(0x1f6, (sect >> 24) | 0xE0);
  outb(0x1f7, 0x30);
  wait_disk();
  for (int i = 0; i < SECTSIZE / 4; i ++) {
    outl(0x1f0, ((uint32_t *)buf)[i]);
  }
}

void copy_from_disk(void *buf, int nbytes, int disk_offset) {
  uint32_t cur  = (uint32_t)buf;
  uint32_t ed   = (uint32_t)buf + nbytes;
  uint32_t sect = (disk_offset / SECTSIZE);
  for(; cur < ed; cur += SECTSIZE, sect ++)
    read_disk((void *)cur, sect);
}

void copy_to_disk(const void *buf, int nbytes, int disk_offset) {
  uint32_t cur  = (uint32_t)buf;
  uint32_t ed   = (uint32_t)buf + nbytes;
  uint32_t sect = (disk_offset / SECTSIZE);
  for(; cur < ed; cur += SECTSIZE, sect ++)
    write_disk((const void *)cur, sect);
}

#define BCACHE_NUM 16

typedef struct {
  int valid, no;
  uint8_t buf[BLK_SIZE];
} bcache_t;

static bcache_t blk_cache[BCACHE_NUM];

static bcache_t *bgetcache(uint32_t no) {
  bcache_t *bc = &blk_cache[no % BCACHE_NUM];
  if (bc->valid == 0 || bc->no != no) {
    copy_from_disk(bc->buf, BLK_SIZE, no * BLK_SIZE);
    bc->valid = 1;
    bc->no = no;
  }
  return bc;
}

void bread(void *dst, uint32_t size, uint32_t no, uint32_t off) {
  // read blk no's [off, off+size) to dst, promise off+size<=BLK_SIZE
  assert(size + off <= BLK_SIZE);
  bcache_t *bc = bgetcache(no);
  memcpy(dst, &bc->buf[off], size);
}

void bwrite(const void *src, uint32_t size, uint32_t no, uint32_t off) {
  // write src to blk no's [off, off+size), promise off+size<=BLK_SIZE
  assert(size + off <= BLK_SIZE);
  bcache_t *bc = bgetcache(no);
  memcpy(&bc->buf[off], src, size);
  copy_to_disk(bc->buf, BLK_SIZE, no * BLK_SIZE);
}

void bzero(uint32_t no) {
  bcache_t *bc = bgetcache(no);
  memset(bc->buf, 0, BLK_SIZE);
  copy_to_disk(bc->buf, BLK_SIZE, no * BLK_SIZE);
}
