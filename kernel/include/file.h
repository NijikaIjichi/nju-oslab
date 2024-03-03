#ifndef __MY_FILE__
#define __MY_FILE__

#define O_RDONLY  0x000
#define O_WRONLY  0x001
#define O_RDWR    0x002
#define O_CREATE  0x200
#define O_TRUNC   0x400

#define SEEK_SET 0
#define SEEK_CUR 1
#define SEEK_END 2

#define PIPE_BUF 2048

typedef struct pipe {
  uint32_t nread, nwrite;
  int read_open, write_open;
  char buf[PIPE_BUF];
} pipe_t;

typedef struct file {
  enum {NONE, DISK, DEV, PIPE} type;
  int ref;
  int readable, writable;
  struct {
    struct icache *ip;
    uint32_t offset;
  };
  device_t *dev;
  pipe_t *pipe;
} file_t;

file_t *fs_open(char *path, int mode);
file_t *fs_dup(file_t *file);
int fs_pipe(file_t **f1, file_t **f2);
void fs_close(file_t *file);
uint32_t fs_read(file_t *file, char *buf, uint32_t size);
uint32_t fs_write(file_t *file, const char *buf, uint32_t size);
uint32_t fs_lseek(file_t *file, uint32_t off, int whence);

int load_elf(char *path, char *argv[], PDE **pgtbl, uint32_t *entry, uint32_t *ustack_top, uint32_t ustack_size);

file_t *file_get(int fd);
int fd_alloc(file_t *f);
void fd_close(int fd);

struct stat {
  uint32_t node;
  uint32_t type;
  uint32_t size;
};

void sys_open(TrapFrame *tf);
void sys_write(TrapFrame *tf);
void sys_read(TrapFrame *tf);
void sys_dup(TrapFrame *tf);
void sys_pipe(TrapFrame *tf);
void sys_close(TrapFrame *tf);
void sys_lseek(TrapFrame *tf);
void sys_mkdir(TrapFrame *tf);
void sys_chdir(TrapFrame *tf);
void sys_unlink(TrapFrame *tf);
void sys_fstat(TrapFrame *tf);

#endif
