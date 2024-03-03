#include "sys.h"
#include "../kernel/include/sys.h"

#define MAKE_SYSCALL(name) \
  .globl name; name: leal 4(%esp), %eax; pushl %eax; pushl $SYS_##name; call syscall; addl $8, %esp; ret;

SYSCALLS(MAKE_SYSCALL)
