#ifndef __PROC__
#define __PROC__

#define MAX_OPEN_FILE 32

typedef struct proc {
  int pid, intena, noff, exit_code, killed;
  enum {UNUSED = 0, UNINIT, RUNNING, RUNNABLE, SLEEPING, ZOMBIE} status;
  void *chan;
  void *kstack;
  TrapFrame *ctx;
  struct proc *parent;
  struct file *files[MAX_OPEN_FILE];
  struct icache *pwd;
  uint32_t brk;
} proc_t;

void yield();
void sleep(void *chan);
void wakeup(void *chan);
void load_proc(TrapFrame *ctx, PDE *pgtbl, uint32_t entry, uint32_t ustack_top);

void sys_fork(TrapFrame *tf);
void sys_wait(TrapFrame *tf);
void sys_exit(TrapFrame *tf);
void sys_sleep(TrapFrame *tf);
void sys_exec(TrapFrame *tf);
void sys_kill(TrapFrame *tf);
void sys_brk(TrapFrame *tf);
void sys_getpid(TrapFrame *tf);

#endif
