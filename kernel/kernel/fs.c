#include "x86.h"
#include "device.h"
#include "fs.h"
#include "lib.h"
#include "proc.h"

#define INODE_NUM  128
#define BCACHE_NUM 16

typedef struct {uint8_t buf[BLK_SIZE];} blk_t;
typedef struct {uint32_t start, empty;} sb_t;
typedef struct {uint32_t next;} empty_t;
typedef struct {uint32_t no; blk_t blk;} bcache_t;

static void bread(void *dst, uint32_t size, uint32_t off, uint32_t no);
static void bwrite(const void *src, uint32_t size, uint32_t off, uint32_t no);
static void sb_update();
static void iupdate(icache_t *ip);
static uint32_t balloc();
static void bfree(uint32_t no);
static void bzero(uint32_t no);
static const char* skipelem(const char *path, char *name);
static icache_t *iget(int no);
static uint32_t iwalk(icache_t *ip, int no);
static icache_t *dirlookup(icache_t *dp, const char *name, uint32_t *off);

sb_t sb;
icache_t inode_cache[INODE_NUM];
bcache_t blk_cache[BCACHE_NUM];
uint32_t root;

void init_fs() {
  extern proc_t *curr;
  for (int i = 0; i < BCACHE_NUM; ++i) {
    blk_cache[i].no = -1;
  }
  bread(&sb, sizeof(sb), 0, 0);
  root = sb.start;
  curr->pwd = iget(root);
}

static icache_t *iget(int no) {
  icache_t *empty = NULL;
  push_off();
  for (int i = 0; i < INODE_NUM; ++i) {
    icache_t *p = &inode_cache[i];
    if (p->ref > 0 && p->no == no) {
      p->ref++;
      pop_off();
      return p;
    }
    if (p->ref == 0) empty = p;
  }
  assert(empty);
  empty->ref++;
  empty->no = no;
  empty->del = 0;
  pop_off();
  bread(&empty->node, sizeof(inode_t), 0, no);
  return empty;
}

icache_t *idup(icache_t *ip) {
  push_off();
  ip->ref++;
  pop_off();
  return ip;
}

void iput(icache_t *ip) {
  push_off();
  ip->ref--;
  if (ip->ref == 0 && ip->del) {
    itrunc(ip);
    bfree(ip->no);
  }
  pop_off();
}

void itrunc(icache_t *ip) {
  static uint32_t buf2[N2], buf3[N2];
  push_off();
  for (int i = 0; i < N1; ++i) {
    if (ip->node.addrs[i]) {
      bfree(ip->node.addrs[i]);
      ip->node.addrs[i] = 0;
    }
  }
  if (ip->node.addrs[N1]) {
    uint32_t n2 = ip->node.addrs[N1];
    bread(buf2, BLK_SIZE, 0, n2);
    for (int i = 0; i < N2; ++i) {
      if (buf2[i]) {
        bfree(buf2[i]);
      }
    }
    bfree(n2);
    ip->node.addrs[N1] = 0;
  }
  if (ip->node.addrs[N1 + 1]) {
    uint32_t n3 = ip->node.addrs[N1 + 1];
    bread(buf2, BLK_SIZE, 0, n3);
    for (int i = 0; i < N2; ++i) {
      if (buf2[i]) {
        bread(buf3, BLK_SIZE, 0, buf2[i]);
        for (int j = 0; j < N2; ++j) {
          if (buf3[j]) {
            bfree(buf3[j]);
          }
        }
        bfree(buf2[i]);
      }
    }
    bfree(n3);
    ip->node.addrs[N1 + 1] = 0;
  }
  ip->node.size = 0;
  iupdate(ip);
  pop_off();
}

icache_t *iopen(const char *path, icache_t **parent, int create, int type, int dev, uint32_t *off) {
  icache_t *ip, *next;
  char name[MAX_NAME + 1];
  extern proc_t *curr;
  if (path[0] == '/') {
    ip = iget(root);
  } else {
    ip = idup(curr->pwd);
  }
  while ((path = skipelem(path, name)) != NULL) {
    push_off();
    if (ip->node.type != TYPE_DIR) {
      iput(ip);
      pop_off();
      return NULL;
    }
    if (parent && *path == 0) {
      *parent = idup(ip);
    }
    if ((next = dirlookup(ip, name, off)) == NULL) {
      if (*path == 0 && create) {
        dirent_t dir;
        dir.node = balloc();
        strcpy(dir.name, name);
        if (off) *off = ip->node.size;
        iwrite(ip, &dir, sizeof(dir), ip->node.size);
        next = iget(dir.node);
        next->node.type = type;
        next->node.device = dev;
        iupdate(next);
        if (type == TYPE_DIR) {
          dir.node = next->no;
          strcpy(dir.name, ".");
          iwrite(next, &dir, sizeof(dir), next->node.size);
          dir.node = ip->no;
          strcpy(dir.name, "..");
          iwrite(next, &dir, sizeof(dir), next->node.size);
        }
      } else {
        iput(ip);
        pop_off();
        return NULL;
      }
    }
    iput(ip);
    pop_off();
    ip = next;
  }
  return ip;
}

static icache_t *dirlookup(icache_t *dp, const char *name, uint32_t *off) {
  dirent_t dir;
  assert(dp->node.type == TYPE_DIR);
  push_off();
  for (uint32_t i = 0; i < dp->node.size; i += sizeof(dirent_t)) {
    iread(dp, &dir, sizeof(dir), i);
    if (dir.node != 0 && strcmp(dir.name, name) == 0) {
      if (off) *off = i;
      icache_t *ip = iget(dir.node);
      pop_off();
      return ip;
    }
  }
  pop_off();
  return NULL;
}

uint32_t iread(icache_t *ip, void *buf, uint32_t size, uint32_t off) {
  uint32_t i = 0;
  uint8_t *bytes = buf;
  push_off();
  while (i < size && off < ip->node.size) {
    uint32_t n = MIN(size - i, ip->node.size - off);
    n = MIN(n, BLK_SIZE - off % BLK_SIZE);
    bread(bytes, n, off % BLK_SIZE, iwalk(ip, off / BLK_SIZE));
    i += n;
    bytes += n;
    off += n;
  }
  pop_off();
  return i;
}

uint32_t iwrite(icache_t *ip, const void *buf, uint32_t size, uint32_t off) {
  uint32_t i = 0;
  const uint8_t *bytes = buf;
  push_off();
  while (i < size) {
    uint32_t n = MIN(size - i, BLK_SIZE - off % BLK_SIZE);
    bwrite(bytes, n, off % BLK_SIZE, iwalk(ip, off / BLK_SIZE));
    i += n;
    bytes += n;
    off += n;
  }
  if (off > ip->node.size) {
    ip->node.size = off;
    iupdate(ip);
  }
  pop_off();
  return i;
}

static void sb_update() {
  bwrite(&sb, sizeof(sb), 0, 0);
}

static void iupdate(icache_t *ip) {
  bwrite(&ip->node, sizeof(inode_t), 0, ip->no);
}

static void bread(void *dst, uint32_t size, uint32_t off, uint32_t no) {
  assert(size + off <= BLK_SIZE);
  push_off();
  bcache_t *b = &blk_cache[no % BCACHE_NUM];
  if (b->no != no) {
    readSect(&b->blk.buf, no + BLK_OFF);
    b->no = no;
  }
  memcpy(dst, &b->blk.buf[off], size);
  pop_off();
}

static void bwrite(const void *src, uint32_t size, uint32_t off, uint32_t no) {
  assert(size + off <= BLK_SIZE);
  push_off();
  bcache_t *b = &blk_cache[no % BCACHE_NUM];
  if (b->no != no) {
    readSect(&b->blk.buf, no + BLK_OFF);
    b->no = no;
  }
  memcpy(&b->blk.buf[off], src, size);
  writeSect(&b->blk.buf, no + BLK_OFF);
  pop_off();
}

static uint32_t balloc() {
  push_off();
  int no = sb.empty;
  assert(no);
  empty_t e;
  bread(&e, sizeof(e), 0, no);
  sb.empty = e.next;
  sb_update();
  pop_off();
  bzero(no);
  return no;
}

static void bfree(uint32_t no) {
  assert(no > 0);
  push_off();
  int next = sb.empty;
  empty_t e = {.next = next};
  bwrite(&e, sizeof(e), 0, no);
  sb.empty = no;
  sb_update();
  pop_off();
}

static void bzero(uint32_t no) {
  static const char zero[512];
  bwrite(zero, sizeof(zero), 0, no);
}

static const char* skipelem(const char *path, char *name) {
  const char *s;
  int len;
  while (*path == '/') path++;
  if (*path == 0) return 0;
  s = path;
  while(*path != '/' && *path != 0) path++;
  len = path - s;
  if (len >= MAX_NAME) {
    memcpy(name, s, MAX_NAME);
    name[MAX_NAME] = 0;
  } else {
    memcpy(name, s, len);
    name[len] = 0;
  }
  while (*path == '/') path++;
  return path;
}

static uint32_t iwalk(icache_t *ip, int no) {
  if (no < N1) {
    if (ip->node.addrs[no] == 0) {
      ip->node.addrs[no] = balloc();
      iupdate(ip);
    }
    return ip->node.addrs[no];
  }
  no -= N1;
  if (no < N2) {
    uint32_t node = 0;
    if (ip->node.addrs[N1] == 0) {
      ip->node.addrs[N1] = balloc();
      iupdate(ip);
    }
    bread(&node, sizeof(node), no * sizeof(node), ip->node.addrs[N1]);
    if (node == 0) {
      node = balloc();
      bwrite(&node, sizeof(node), no * sizeof(node), ip->node.addrs[N1]);
    }
    return node;
  }
  no -= N2;
  if (no < N3) {
    uint32_t n31 = 0, n32 = 0;
    if (ip->node.addrs[N1 + 1] == 0) {
      ip->node.addrs[N1 + 1] = balloc();
      iupdate(ip);
    }
    bread(&n31, sizeof(n31), (no / N2) * sizeof(n31), ip->node.addrs[N1 + 1]);
    if (n31 == 0) {
      n31 = balloc();
      bwrite(&n31, sizeof(n31), (no / N2) * sizeof(n31), ip->node.addrs[N1 + 1]);
    }
    bread(&n32, sizeof(n32), (no % N2) * sizeof(n32), n31);
    if (n32 == 0) {
      n32 = balloc();
      bwrite(&n32, sizeof(n32), (no % N2) * sizeof(n32), n31);
    }
    return n32;
  }
  assert(0);
}
