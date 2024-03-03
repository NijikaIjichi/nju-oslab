#include "x86.h"
#include "device.h"
#include "sys.h"
#include "proc.h"
#include "file.h"
#include "lib.h"
#include "fs.h"

#define TOTAL_FILE   128

file_t file_table[TOTAL_FILE];

static file_t *fs_alloc() {
  file_t *file = NULL;
  push_off();
  for (int i = 0; i < TOTAL_FILE; ++i) {
    if (file_table[i].ref == 0) {
      file = &file_table[i];
      file->ref++;
      break;
    }
  }
  pop_off();
  return file;
}

file_t *fs_open(char *path, int mode) {
  file_t *file = NULL;
  device_t *dev = NULL;
  icache_t *ip = iopen(path, NULL, mode & O_CREATE, TYPE_FILE, 0, NULL);
  if (ip == NULL) goto bad;
  if (ip->node.type == TYPE_DIR && mode != O_RDONLY) goto bad;
  if (ip->node.type == TYPE_FILE && (mode & O_TRUNC)) {
    itrunc(ip);
  }
  if (ip->node.type == TYPE_DEV) {
    dev = device_lookup(ip->node.device);
    if (dev == NULL) goto bad;
  }
  file = fs_alloc();
  if (file == NULL) goto bad;
  file->readable = !(mode & O_WRONLY);
  file->writable = (mode & O_WRONLY) || (mode & O_RDWR);
  if (dev) {
    file->type = DEV;
    file->dev = dev;
    file->ip = ip;
  } else {
    file->type = DISK;
    file->offset = 0;
    file->ip = ip;
  }
  return file;
bad:
  if (ip) iput(ip);
  return NULL;
}

file_t *fs_dup(file_t *file) {
  push_off();
  file->ref++;
  pop_off();
  return file;
}

int fs_pipe(file_t **f1, file_t **f2) {
  *f1 = fs_alloc();
  if (*f1 == NULL) return -1;
  *f2 = fs_alloc();
  if (*f2 == NULL) {
    fs_close(*f1);
    return -1;
  }
  pipe_t *pipe = kalloc();
  pipe->nread = pipe->nwrite = 0;
  pipe->read_open = pipe->write_open = 1;
  (*f1)->type = (*f2)->type = PIPE;
  (*f1)->readable = (*f2)->writable = 1;
  (*f1)->writable = (*f2)->readable = 0;
  (*f1)->pipe = (*f2)->pipe = pipe;
  return 0;
}

void fs_close(file_t *file) {
  push_off();
  file->ref--;
  if (file->ref == 0) {
    if (file->type == PIPE) {
      if (file->writable) {
        file->pipe->write_open = 0;
        wakeup(&file->pipe->nread);
      } else {
        file->pipe->read_open = 0;
        wakeup(&file->pipe->nwrite);
      }
      if (file->pipe->write_open == 0 && file->pipe->read_open == 0) {
        kfree(file->pipe);
      }
      file->pipe = NULL;
    } else if (file->type == DISK) {
      iput(file->ip);
      file->ip = NULL;
      file->offset = 0;
    }
    file->type = NONE;
  }
  pop_off();
}

static uint32_t pipe_read(pipe_t *pipe, char *buf, uint32_t size) {
  uint32_t i = 0;
  push_off();
  while (pipe->nread == pipe->nwrite && pipe->write_open) {
    sleep(&pipe->nread);
  }
  while (i < size && pipe->nread < pipe->nwrite) {
    buf[i++] = pipe->buf[pipe->nread++ % PIPE_BUF];
  }
  wakeup(&pipe->nwrite);
  pop_off();
  return i;
}

uint32_t fs_read(file_t *file, char *buf, uint32_t size) {
  if (file->readable == 0) return -1;
  switch (file->type) {
  case NONE: assert(0);
  case DISK: {
    push_off();
    uint32_t r = iread(file->ip, buf, size, file->offset);
    file->offset += r;
    pop_off();
    return r;
  }
  case DEV: return file->dev->read(buf, size);
  case PIPE: return pipe_read(file->pipe, buf, size);
  }
  assert(0);
  return -1;
}

static uint32_t pipe_write(pipe_t *pipe, const char *buf, uint32_t size) {
  uint32_t i = 0;
  push_off();
  while (pipe->nwrite == pipe->nread + PIPE_BUF && pipe->read_open) {
    sleep(&pipe->nwrite);
  }
  while (i < size && pipe->nwrite < pipe->nread + PIPE_BUF) {
    pipe->buf[pipe->nwrite++ % PIPE_BUF] = buf[i++];
  }
  wakeup(&pipe->nread);
  pop_off();
  return i;
}

uint32_t fs_write(file_t *file, const char *buf, uint32_t size) {
  if (file->writable == 0) return -1;
  switch (file->type) {
  case NONE: assert(0);
  case DISK: {
    push_off();
    uint32_t r = iwrite(file->ip, buf, size, file->offset);
    file->offset += r;
    pop_off();
    return r;
  }
  case DEV: return file->dev->write(buf, size);
  case PIPE: return pipe_write(file->pipe, buf, size);
  }
  assert(0);
  return -1;
}

uint32_t fs_lseek(file_t *file, uint32_t off, int whence) {
  if (file->type == NONE) assert(0);
  else if (file->type != DISK) return -1;
  push_off();
  switch (whence) {
  case SEEK_SET: file->offset = off; break;
  case SEEK_CUR: file->offset += off; break;
  case SEEK_END: file->offset = file->ip->node.size + off; break;
  }
  file->offset = MIN(file->offset, file->ip->node.size);
  pop_off();
  return file->offset;
}

void sys_open(TrapFrame *tf) {
  file_t *f = fs_open((char*)tf->GPR1, tf->GPR2);
  if (f == NULL) {
    tf->eax = -1;
    return;
  }
  int fd = fd_alloc(f);
  if (fd < 0) {
    fs_close(f);
    tf->eax = -1;
    return;
  }
  tf->eax = fd;
}

void sys_write(TrapFrame *tf) {
  file_t *f = file_get(tf->GPR1);
  if (f == NULL) {
    tf->eax = -1;
  } else {
    tf->eax = fs_write(f, (char*)tf->GPR2, tf->GPR3);
  }
}

void sys_read(TrapFrame *tf) {
  file_t *f = file_get(tf->GPR1);
  if (f == NULL) {
    tf->eax = -1;
  } else {
    tf->eax = fs_read(f, (char*)tf->GPR2, tf->GPR3);
  }
}

void sys_dup(TrapFrame *tf) {
  file_t *f = file_get(tf->GPR1);
  if (f == NULL) {
    tf->eax = -1;
    return;
  }
  int fd = fd_alloc(fs_dup(f));
  if (fd < 0) {
    fs_close(f);
    tf->eax = -1;
    return;
  }
  tf->eax = fd;
}

void sys_pipe(TrapFrame *tf) {
  file_t *f1, *f2;
  if (fs_pipe(&f1, &f2) < 0) {
    tf->eax = -1;
    return;
  }
  int fd1 = fd_alloc(f1);
  if (fd1 < 0) {
    fs_close(f1);
    tf->eax = -1;
    return;
  }
  int fd2 = fd_alloc(f2);
  if (fd2 < 0) {
    fd_close(fd1);
    fs_close(f2);
    tf->eax = -1;
    return;
  }
  int *fds = (int*)(tf->GPR1);
  fds[0] = fd1;
  fds[1] = fd2;
  tf->eax = 0;
}

void sys_close(TrapFrame *tf) {
  int fd = tf->GPR1;
  fd_close(fd);
}

void sys_lseek(TrapFrame *tf) {
  file_t *f = file_get(tf->GPR1);
  if (f == NULL) {
    tf->eax = -1;
    return;
  }
  tf->eax = fs_lseek(f, tf->GPR2, tf->GPR3);
}

void sys_mkdir(TrapFrame *tf) {
  push_off();
  icache_t *ip = iopen((char*)(tf->GPR1), NULL, 1, TYPE_DIR, 0, NULL);
  if (ip) {
    iput(ip);
    tf->eax = 0;
  } else tf->eax = -1;
  pop_off();
}

void sys_chdir(TrapFrame *tf) {
  extern proc_t *curr;
  push_off();
  icache_t *ip = iopen((char*)(tf->GPR1), NULL, 0, 0, 0, NULL);
  if (ip) {
    if (ip->node.type == TYPE_DIR) {
      iput(curr->pwd);
      curr->pwd = ip;
      pop_off();
      tf->eax = 0;
      return;
    } else iput(ip);
  }
  pop_off();
  tf->eax = -1;
}

static int is_dir_empty(icache_t *dp) {
  dirent_t d;
  for (uint32_t i = 2 * sizeof(d); i < dp->node.size; i += sizeof(d)) {
    iread(dp, &d, sizeof(d), i);
    if (d.node != 0) return 0;
  }
  return 1;
}

void sys_unlink(TrapFrame *tf) {
  icache_t *ip = NULL, *dp = NULL;
  uint32_t off;
  dirent_t dir;
  push_off();
  ip = iopen((char*)(tf->GPR1), &dp, 0, 0, 0, &off);
  if (!ip || !dp || off == 0 || off == sizeof(dirent_t) || (ip && ip->node.type == TYPE_DIR && !is_dir_empty(ip))) {
    if (ip) iput(ip);
    if (dp) iput(dp);
    tf->eax = -1;
  } else {
    ip->del = 1;
    memset(&dir, 0, sizeof(dir));
    iwrite(dp, &dir, sizeof(dir), off);
    iput(ip);
    iput(dp);
    tf->eax = 0;
  }
  pop_off();
}

void sys_fstat(TrapFrame *tf) {
  struct stat *s = (void*)(tf->GPR2);
  file_t *f = file_get(tf->GPR1);
  if (f && (f->type == DISK || f->type == DEV)) {
    push_off();
    s->node = f->ip->no;
    s->type = f->ip->node.type;
    s->size = f->ip->node.size;
    pop_off();
    tf->eax = 0;
  } else tf->eax = -1;
}
