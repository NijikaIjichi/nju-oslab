#include <stdint.h>
#include <stddef.h>
#include <sys/mman.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define DISK_SIZE (128 * 1024 * 1024)
#define BLK_SIZE  512
#define BLK_OFF   256
#define BLK_NUM   ((DISK_SIZE / BLK_SIZE) - BLK_OFF)

#define N1 16
#define N2 (BLK_SIZE / sizeof(uint32_t))
#define N3 (N2 * N2)
#define MAX_SIZE ((N1 + N2 + N3) * BLK_SIZE)

#define MAX_NAME (31 - sizeof(uint32_t))

#define TYPE_NONE 0
#define TYPE_FILE 1
#define TYPE_DIR  2
#define TYPE_DEV  3

typedef struct {uint8_t buf[BLK_SIZE];} blk_t;
typedef struct {uint32_t start, empty;} sb_t;
typedef struct {uint32_t type, device, size, addrs[N1 + 2];} inode_t;
typedef struct {uint32_t next;} empty_t;
typedef struct {uint32_t node; char name[MAX_NAME + 1];} dirent_t;

blk_t *disk;
sb_t *sb;
inode_t *root;

#define BLK(n) (&disk[(n) + BLK_OFF])
#define NO(b)  ((blk_t*)(b) - &disk[BLK_OFF])

#define MOV_E(s, v) do { if ((s) == 0) (s) = (v); } while (0)
#define MIN(a, b) ((a) > (b) ? (b) : (a))
#define MAX(a, b) ((a) < (b) ? (b) : (a))

void init_disk();
void *alloc_block();
blk_t *walk(inode_t *file, uint32_t blk_no);
void append(inode_t *file, const void *buf, uint32_t size);
void dirent_init(dirent_t *dir, int node, const char *name);
void safe_chdir(const char *path);
void write_dir(inode_t *node, const char *name, inode_t *parent);
void write_file(inode_t *node, const char *name);

const char *dev_name[] = {"tty", "serial", "null", NULL};

int main(int argc, char *argv[]) {
  if (argc < 3) {
    fprintf(stderr, "usage: mkfs [target] [source].\n");
    exit(1);
  }
  char *target = argv[1];
  char *source = argv[2];
  int tfd = open(target, O_RDWR);
  if (tfd < 0) {
    fprintf(stderr, "open target error.\n");
    exit(1);
  }
  if (ftruncate(tfd, DISK_SIZE) < 0) {
    fprintf(stderr, "truncate error.\n");
    exit(1);
  }
  disk = mmap(NULL, DISK_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, tfd, 0);
  init_disk();
  write_dir(root, source, root);
  munmap(disk, DISK_SIZE);
  close(tfd);
  return 0;
}

void init_disk() {
  sb = (sb_t*)BLK(0);
  sb->empty = 1;
  int i = 1;
  for (; i < BLK_NUM - 1; ++i) {
    ((empty_t*)BLK(i))->next = i + 1;
  }
  ((empty_t*)BLK(i))->next = 0;
  root = alloc_block();
  sb->start = NO(root);
}

void *alloc_block() {
  int blk = sb->empty;
  if (blk == 0) {
    fprintf(stderr, "disk to small.\n");
    exit(1);
  }
  empty_t *em = (empty_t*)BLK(blk);
  sb->empty = em->next;
  memset(em, 0, BLK_SIZE);
  return em;
}

blk_t *walk(inode_t *file, uint32_t blk_no) {
  if (blk_no < N1) {
    MOV_E(file->addrs[blk_no], NO(alloc_block()));
    return BLK(file->addrs[blk_no]);
  }
  blk_no -= N1;
  if (blk_no < N2) {
    MOV_E(file->addrs[N1], NO(alloc_block()));
    uint32_t *n2 = (uint32_t*)BLK(file->addrs[N1]);
    MOV_E(n2[blk_no], NO(alloc_block()));
    return BLK(n2[blk_no]);
  }
  blk_no -= N2;
  if (blk_no < N3) {
    MOV_E(file->addrs[N1 + 1], NO(alloc_block()));
    uint32_t *n31 = (uint32_t*)BLK(file->addrs[N1 + 1]);
    MOV_E(n31[blk_no / N2], NO(alloc_block()));
    uint32_t *n32 = (uint32_t*)BLK(n31[blk_no / N2]);
    MOV_E(n32[blk_no % N2], NO(alloc_block()));
    return BLK(n32[blk_no % N2]);
  }
  fprintf(stderr, "file too big.\n");
  exit(1);
}

void append(inode_t *file, const void *buf, uint32_t size) {
  const uint8_t *bytes = buf;
  if (file->size + size > MAX_SIZE) {
    fprintf(stderr, "file too big.\n");
    exit(1);
  }
  while (size > 0) {
    uint32_t no = file->size / BLK_SIZE;
    uint32_t off = file->size % BLK_SIZE;
    uint32_t n = MIN(size, BLK_SIZE - off);
    blk_t *blk = walk(file, no);
    memcpy(&blk->buf[off], bytes, n);
    bytes += n;
    file->size += n;
    size -= n;
  }
}

void dirent_init(dirent_t *dir, int node, const char *name) {
  if (strlen(name) > MAX_NAME) {
    fprintf(stderr, "name too long.\n");
    exit(1);
  }
  dir->node = node;
  strcpy(dir->name, name);
}

void safe_chdir(const char *path) {
  if (chdir(path) < 0) {
    fprintf(stderr, "chdir error.\n");
    exit(1);
  }
}

void write_dir(inode_t *node, const char *name, inode_t *parent) {
  DIR *dir = opendir(name);
  dirent_t d;
  struct dirent *inner;
  if (dir == NULL) {
    fprintf(stderr, "bad dir.\n");
    exit(1);
  }
  node->type = TYPE_DIR;
  dirent_init(&d, NO(node), ".");
  append(node, &d, sizeof(d));
  dirent_init(&d, NO(parent), "..");
  append(node, &d, sizeof(d));
  safe_chdir(name);
  for (inner = readdir(dir); inner; inner = readdir(dir)) {
    if (inner->d_name[0] == '.') continue;
    inode_t *inner_node = alloc_block();
    dirent_init(&d, NO(inner_node), inner->d_name);
    append(node, &d, sizeof(d));
    if (inner->d_type == DT_REG) {
      write_file(inner_node, inner->d_name);
    } else if (inner->d_type == DT_DIR) {
      write_dir(inner_node, inner->d_name, node);
    } else {
      fprintf(stderr, "unsupported file.\n");
      exit(1);
    }
  }
  safe_chdir("..");
  closedir(dir);
}

void write_file(inode_t *node, const char *name) {
  char buf[512];
  FILE *fp = fopen(name, "r");
  uint32_t n;
  node->type = TYPE_FILE;
  if (fp == NULL) {
    fprintf(stderr, "bad file.\n");
    exit(1);
  }
  while ((n = fread(buf, 1, 512, fp)) > 0) {
    append(node, buf, n);
  }
  fclose(fp);
}
