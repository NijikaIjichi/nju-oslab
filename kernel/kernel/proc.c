#include "x86.h"
#include "device.h"
#include "sys.h"
#include "proc.h"
#include "file.h"
#include "lib.h"
#include "fs.h"

#define MAX_PROC_NUM   64
#define STACK_SIZE     (PAGE_SIZE * 4)
#define USER_STACK_TOP 0xc0000000

proc_t proc_pool[MAX_PROC_NUM];
proc_t *curr = NULL, *init = NULL;

file_t *file_get(int fd) {
  if (fd < 0 || fd >= MAX_OPEN_FILE || curr->files[fd] == NULL) return NULL;
  return curr->files[fd]; 
}

int fd_alloc(file_t *f) {
  int fd = 0;
  push_off();
  for (; fd < MAX_OPEN_FILE; ++fd) {
    if (curr->files[fd] == NULL) {
      curr->files[fd] = f;
      break;
    }
  }
  pop_off();
  return fd < MAX_OPEN_FILE ? fd : -1;
}

void fd_close(int fd) {
  if (fd >= 0 && fd < MAX_OPEN_FILE && curr->files[fd] != NULL) {
    fs_close(curr->files[fd]);
    curr->files[fd] = NULL;
  }
}

void push_off() {
  int i = is_interrupt_on();
  disableInterrupt();
  assert(curr);
  if (curr->noff == 0) curr->intena = i;
  curr->noff++;
}

void pop_off() {
  assert(curr && !is_interrupt_on());
  curr->noff--;
  if (curr->noff == 0 && curr->intena) enableInterrupt();
}

proc_t *alloc_proc() {
  static volatile int pid = 1;
  proc_t *proc = NULL;
  for (int i = 0; !proc && i < MAX_PROC_NUM; ++i) {
    push_off();
    if (proc_pool[i].status == UNUSED) {
      proc = &proc_pool[i];
      memset(proc, 0, sizeof(proc_t));
      proc->pid = pid++;
      proc->status = UNINIT;
      proc->kstack = (void*)(((uint32_t)kalloc()) + PAGE_SIZE);
      proc->ctx = (TrapFrame *)(proc->kstack) - 1;
    }
    pop_off();
  }
  return proc;
}

TrapFrame *schedule(TrapFrame *tf) {
  extern TSS tss;
  assert(!is_interrupt_on());
  int i = curr - proc_pool;
  curr->ctx = tf;
  curr->ctx->cr3 = read_cr3();
  assert(curr->status != RUNNING);
  for (i = (i + 1) % MAX_PROC_NUM; ; i = (i + 1) % MAX_PROC_NUM) {
    proc_t *proc = &proc_pool[i];
    if (proc->status == RUNNABLE) {
      proc->status = RUNNING;
      curr = proc;
      tss.esp0 = (uint32_t)proc->kstack;
      write_cr3(proc->ctx->cr3);
      return proc->ctx;
    }
  }
}

void load_proc(TrapFrame *ctx, PDE *pgtbl, uint32_t entry, uint32_t ustack_top) {
  ctx->cr3 = (uint32_t)pgtbl;
  ctx->cs = USEL(SEG_UCODE);
  ctx->ds = USEL(SEG_UDATA);
  ctx->eflags = 0x202;
  ctx->eip = entry;
  ctx->esp = ustack_top;
  ctx->ss = USEL(SEG_UDATA);
}

void init_proc() {
  curr = &proc_pool[0]; // kernel idle
  curr->status = RUNNING;
  curr->kstack = (void*)KMEM;
  curr->pid = 0;
  curr->parent = NULL;
}

void load_init_proc() {
  PDE *pgtbl;
  char *argv[] = {"init", NULL};
  uint32_t entry, ustack = USER_STACK_TOP;
  assert(load_elf("init", argv, &pgtbl, &entry, &ustack, STACK_SIZE) == 0);
  init = alloc_proc();
  assert(init);
  load_proc(init->ctx, pgtbl, entry, ustack);
  init->status = RUNNABLE;
  init->pwd = idup(curr->pwd);
}

void yield() {
  push_off();
  curr->status = RUNNABLE;
  INT(0x81);
  pop_off();
}

void sleep(void *chan) {
  assert(!is_interrupt_on());
  curr->chan = chan;
  curr->status = SLEEPING;
  INT(0x81);
  curr->chan = NULL;
}

void wakeup(void *chan) {
  for (proc_t *p = proc_pool; p < &proc_pool[MAX_PROC_NUM]; ++p) {
    push_off();
    if (p->status == SLEEPING && p->chan == chan) {
      p->status = RUNNABLE;
    }
    pop_off();
  }
}

void sys_fork(TrapFrame *tf) {
  proc_t *p = alloc_proc();
  PDE *upgdir = NULL;
  if (!p) {
    tf->eax = -1;
    return;
  }
  p->parent = curr;
  tf->eax = p->pid;
  upgdir = uvm_protect();
  uvm_copy(upgdir, (void*)read_cr3());
  *(p->ctx) = *tf;
  p->ctx->cr3 = (uint32_t)upgdir;
  p->ctx->eax = 0;
  p->pwd = idup(curr->pwd);
  p->brk = curr->brk;
  for (int i = 0; i < MAX_OPEN_FILE; ++i) {
    if (curr->files[i]) {
      p->files[i] = fs_dup(curr->files[i]);
    }
  }
  push_off();
  p->status = RUNNABLE;
  pop_off();
}

void sys_wait(TrapFrame *tf) {
  int has_child = 0, found = 0;
  push_off();
  for (;;) {
    for (proc_t *p = proc_pool; !found && p < &proc_pool[MAX_PROC_NUM]; ++p) {
      if (p->status != UNUSED && p->parent == curr) {
        has_child = 1;
        if (p->status == ZOMBIE) {
          found = 1;
          tf->eax = p->pid;
          if (tf->GPR1) *(int*)(tf->GPR1) = p->exit_code;
          p->parent = NULL;
          uvm_dealloc((PDE*)p->ctx->cr3, PHY_MEM, USER_STACK_TOP - PHY_MEM);
          uvm_free((PDE*)p->ctx->cr3);
          kfree((void*)((uint32_t)(p->kstack) - PAGE_SIZE));
          p->status = UNUSED;
          memset(p, 0, sizeof(proc_t));
        }
      }
    }
    if (has_child && !found) {
      sleep(curr);
    } else break;
  }
  pop_off();
  if (!has_child) tf->eax = -1;
}

void sys_exit(TrapFrame *tf) {
  assert(curr != init);
  disableInterrupt();
  for (int i = 0; i < MAX_OPEN_FILE; ++i) {
    if (curr->files[i]) {
      fs_close(curr->files[i]);
      curr->files[i] = NULL;
    }
  }
  for (proc_t *p = proc_pool; p < &proc_pool[MAX_PROC_NUM]; ++p) {
    push_off();
    if (p->status != UNUSED && p->parent == curr) {
      p->parent = init;
    }
    pop_off();
  }
  iput(curr->pwd);
  curr->exit_code = tf->GPR1;
  curr->status = ZOMBIE;
  wakeup(curr->parent);
  wakeup(init);
  INT(0x81);
  assert(0);
}

void sys_sleep(TrapFrame *tf) {
  extern volatile int tick;
  int start = tick, sleep_time = tf->GPR1;
  push_off();
  while (tick - start < sleep_time) {
    sleep((void*)&tick);
  }
  pop_off();
}

void sys_exec(TrapFrame *tf) {
  PDE *pgdir, *currpg;
  uint32_t entry, ustack = USER_STACK_TOP;
  if (load_elf((char*)tf->GPR1, (char**)tf->GPR2, &pgdir, &entry, &ustack, STACK_SIZE) < 0) {
    tf->eax = -1;
    return;
  }
  push_off();
  currpg = (void*)read_cr3();
  write_cr3((uint32_t)pgdir);
  uvm_dealloc(currpg, PHY_MEM, USER_STACK_TOP - PHY_MEM);
  uvm_free(currpg);
  curr->brk = 0;
  load_proc(tf, pgdir, entry, ustack);
  pop_off();
}

void sys_kill(TrapFrame *tf) {
  int pid = tf->GPR1, found = 0;
  for (proc_t *p = proc_pool; p < &proc_pool[MAX_PROC_NUM]; ++p) {
    push_off();
    if (p->status != UNUSED && p->status != UNINIT && p->status != ZOMBIE && p->pid == pid) {
      p->killed = 1;
      found = 1;
      if (p->status == SLEEPING) p->status = RUNNABLE;
    }
    pop_off();
  }
  tf->eax = found ? 0 : -1;
}

void sys_brk(TrapFrame *tf) {
  uint32_t new_brk = tf->GPR1;
  push_off();
  if (curr->brk == 0) {
    if ((new_brk & PAGE_MASK) == 0) {
      curr->brk = new_brk;
      tf->eax = 0;
    } else tf->eax = -1;
  } else {
    if (new_brk > curr->brk) {
      uvm_alloc((PDE*)read_cr3(), curr->brk, PAGE_UP(new_brk) - curr->brk, PTE_U | PTE_W);
      curr->brk = PAGE_UP(new_brk);
    } else if (PAGE_UP(new_brk) < curr->brk) {
      uvm_dealloc((PDE*)read_cr3(), PAGE_UP(new_brk), curr->brk - PAGE_UP(new_brk));
      curr->brk = PAGE_UP(new_brk);
    }
    tf->eax = 0;
  }
  pop_off();
}

void sys_getpid(TrapFrame *tf) {
  tf->eax = curr->pid;
}
