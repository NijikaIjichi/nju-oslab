#ifndef __SYS_CALL__
#define __SYS_CALL__

#define SYS_write  0
#define SYS_read   1
#define SYS_yield  2
#define SYS_fork   3
#define SYS_sleep  4
#define SYS_exit   5
#define SYS_wait   6
#define SYS_exec   7
#define SYS_open   8
#define SYS_dup    9
#define SYS_pipe   10
#define SYS_close  11
#define SYS_lseek  12
#define SYS_kill   13
#define SYS_mkdir  14
#define SYS_chdir  15
#define SYS_unlink 16
#define SYS_fstat  17
#define SYS_brk    18
#define SYS_getpid 19

#define STD_IN  0
#define STD_OUT 1
#define STD_ERR 2

#define GPR0 eax
#define GPR1 ecx
#define GPR2 edx
#define GPR3 ebx
#define GPR4 esi
#define GPR5 edi

#endif
