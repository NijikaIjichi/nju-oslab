#ifndef __SYSNUM_H__
#define __SYSNUM_H__

// compulsory syscall
#define SYS_write     0
#define SYS_read      1
#define SYS_brk       2
#define SYS_sleep     3
#define SYS_exec      4
#define SYS_getpid    5
#define SYS_yield     6
#define SYS_fork      7
#define SYS_exit      8
#define SYS_wait      9
#define SYS_sem_open  10
#define SYS_sem_p     11
#define SYS_sem_v     12
#define SYS_sem_close 13
#define SYS_open      14
#define SYS_close     15
#define SYS_dup       16
#define SYS_lseek     17
#define SYS_fstat     18
#define SYS_chdir     19
#define SYS_unlink    20

// optional syscall
#define SYS_mmap      21
#define SYS_munmap    22
#define SYS_clone     23
#define SYS_kill      24
#define SYS_cv_open   25
#define SYS_cv_wait   26
#define SYS_cv_sig    27
#define SYS_cv_sigall 28
#define SYS_cv_close  29
#define SYS_pipe      30
#define SYS_link      31
#define SYS_symlink   32

#define NR_SYS        33

#endif
