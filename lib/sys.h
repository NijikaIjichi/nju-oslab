#ifndef __SYS_USER__
#define __SYS_USER__

#define SYSCALLS(_) \
  _(write) \
  _(read) \
  _(yield) \
  _(fork) \
  _(sleep) \
  _(wait) \
  _(exit) \
  _(exec) \
  _(open) \
  _(dup) \
  _(pipe) \
  _(close) \
  _(lseek) \
  _(kill) \
  _(mkdir) \
  _(chdir) \
  _(unlink) \
  _(fstat) \
  _(brk) \
  _(getpid)

#endif
