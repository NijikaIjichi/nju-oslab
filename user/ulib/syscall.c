#include "ulib.h"
#include "sysnum.h"

int syscall(int num, 
            size_t arg1, size_t arg2, size_t arg3, size_t arg4, size_t arg5) {
  int ret;
  asm volatile (
    "int $0x80"
    : "=a"(ret)
    : "a"(num), "b"(arg1), "c"(arg2), "d"(arg3), "S"(arg4), "D"(arg5)
  );
  return ret;
}

int write(int fd, const void *buf, size_t count) {
  return (int)syscall(SYS_write, (size_t)fd, (size_t)buf, (size_t)count, 0, 0);
}

int read(int fd, void *buf, size_t count) {
  return (int)syscall(SYS_read, (size_t)fd, (size_t)buf, (size_t)count, 0, 0);
}

int brk(void *addr) {
  return (int)syscall(SYS_brk, (size_t)addr, 0, 0, 0, 0);
}

void sleep(int ticks) {
  syscall(SYS_sleep, (size_t)ticks, 0, 0, 0, 0);
}

int exec(const char *path, char *const argv[]) {
  return (int)syscall(SYS_exec, (size_t)path, (size_t)argv, 0, 0, 0);
}

int getpid() {
  return (int)syscall(SYS_getpid, 0, 0, 0, 0, 0);
}

void yield() {
  syscall(SYS_yield, 0, 0, 0, 0, 0);
}

int fork() {
  return (int)syscall(SYS_fork, 0, 0, 0, 0, 0);
}

void exit(int status) {
  syscall(SYS_exit, (size_t)status, 0, 0, 0, 0);
  while (1) ;
}

int wait(int *status) {
  return (int)syscall(SYS_wait, (size_t)status, 0, 0, 0, 0);
}

int sem_open(int value) {
  return (int)syscall(SYS_sem_open, (size_t)value, 0, 0, 0, 0);
}

int sem_p(int sem_id) {
  return (int)syscall(SYS_sem_p, (size_t)sem_id, 0, 0, 0, 0);
}

int sem_v(int sem_id) {
  return (int)syscall(SYS_sem_v, (size_t)sem_id, 0, 0, 0, 0);
}

int sem_close(int sem_id) {
  return (int)syscall(SYS_sem_close, (size_t)sem_id, 0, 0, 0, 0);
}

int open(const char *path, int mode) {
  return (int)syscall(SYS_open, (size_t)path, (size_t)mode, 0, 0, 0);
}

int close(int fd) {
  return (int)syscall(SYS_close, (size_t)fd, 0, 0, 0, 0);
}

int dup(int fd) {
  return (int)syscall(SYS_dup, (size_t)fd, 0, 0, 0, 0);
}

uint32_t lseek(int fd, uint32_t off, int whence) {
  return (uint32_t)syscall(SYS_lseek, (size_t)fd, (size_t)off, (size_t)whence, 0, 0);
}

int fstat(int fd, struct stat *st) {
  return (int)syscall(SYS_fstat, (size_t)fd, (size_t)st, 0, 0, 0);
}

int chdir(const char *path) {
  return (int)syscall(SYS_chdir, (size_t)path, 0, 0, 0, 0);
}

int unlink(const char *path) {
  return (int)syscall(SYS_unlink, (size_t)path, 0, 0, 0, 0);
}

// optional syscall

void *mmap() {
  return (void*)syscall(SYS_mmap, 0, 0, 0, 0, 0);
}

void munmap(void *addr) {
  syscall(SYS_munmap, (size_t)addr, 0, 0, 0, 0);
}

int clone(void (*entry)(void*), void *stack, void *arg) {
  return (int)syscall(SYS_clone, (size_t)entry, (size_t)stack, (size_t)arg, 0, 0);
}

int kill(int pid) {
  return (int)syscall(SYS_kill, (size_t)pid, 0, 0, 0, 0);
}

int cv_open() {
  return (int)syscall(SYS_cv_open, 0, 0, 0, 0, 0);
}

int cv_wait(int cv_id, int sem_id) {
  return (int)syscall(SYS_cv_wait, (size_t)cv_id, (size_t)sem_id, 0, 0, 0);
}

int cv_sig(int cv_id) {
  return (int)syscall(SYS_cv_sig, (size_t)cv_id, 0, 0, 0, 0);
}

int cv_sigall(int cv_id) {
  return (int)syscall(SYS_cv_sigall, (size_t)cv_id, 0, 0, 0, 0);
}

int cv_close(int cv_id) {
  return (int)syscall(SYS_cv_close, (size_t)cv_id, 0, 0, 0, 0);
}

int pipe(int fd[2]) {
  return (int)syscall(SYS_pipe, (size_t)fd, 0, 0, 0, 0);
}

int link(const char *oldpath, const char *newpath) {
  return (int)syscall(SYS_link, (size_t)oldpath, (size_t)newpath, 0, 0, 0);
}

int symlink(const char *oldpath, const char *newpath) {
  return (int)syscall(SYS_symlink, (size_t)oldpath, (size_t)newpath, 0, 0, 0);
}
