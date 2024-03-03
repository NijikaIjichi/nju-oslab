#include "klib.h"
#include "cte.h"
#include "proc.h"

#define PROC_NUM 64

static __attribute__((used)) int next_pid = 1;

proc_t pcb[PROC_NUM];
static proc_t *curr = &pcb[0];

void init_proc() {
  // Lab2-1, set status and pgdir
  // Lab2-4, init zombie_sem
  // Lab3-2, set cwd
  curr->status = RUNNING;
  curr->pgdir = vm_curr();
  sem_init(&curr->zombie_sem, 0);
  curr->cwd = iopen("/", TYPE_NONE);
}

proc_t *proc_alloc() {
  // Lab2-1: find a unused pcb from pcb[1..PROC_NUM-1], return NULL if no such one
  //TODO();
  proc_t *p = NULL;
  for (int i = 1; i < PROC_NUM; ++i) {
    if (pcb[i].status == UNUSED) {
      p = &pcb[i];
      break;
    }
  }
  if (!p) return NULL;
  // init ALL attributes of the pcb
  p->pid = next_pid++;
  p->status = UNINIT;
  p->pgdir = vm_alloc();
  p->brk = 0;
  p->kstack = kalloc();
  p->ctx = &p->kstack->ctx;
  p->parent = NULL;
  p->child_num = 0;
  p->exit_code = 0;
  sem_init(&p->zombie_sem, 0);
  memset(p->usems, 0, sizeof(p->usems));
  memset(p->files, 0, sizeof(p->files));
  p->cwd = NULL;
  return p;
}

void proc_free(proc_t *proc) {
  // Lab2-1: free proc's pgdir and kstack and mark it UNUSED
  //TODO();
  proc->status = UNUSED;
}

proc_t *proc_curr() {
  return curr;
}

void proc_run(proc_t *proc) {
  proc->status = RUNNING;
  curr = proc;
  set_cr3(proc->pgdir);
  set_tss(KSEL(SEG_KDATA), (uint32_t)STACK_TOP(proc->kstack));
  irq_iret(proc->ctx);
}

void proc_addready(proc_t *proc) {
  // Lab2-1: mark proc READY
  //TODO();
  proc->status = READY;
}

void proc_yield() {
  // Lab2-1: mark curr proc READY, then int $0x81
  //TODO();
  curr->status = READY;
  INT(0x81);
}

void proc_copycurr(proc_t *proc) {
  // Lab2-2: copy curr proc
  // Lab2-5: dup opened usems
  // Lab3-1: dup opened files
  // Lab3-2: dup cwd
  //TODO();
  vm_copycurr(proc->pgdir);
  proc->brk = curr->brk;
  proc->kstack->ctx = curr->kstack->ctx;
  proc->kstack->ctx.eax = 0;
  for (int i = 0; i < MAX_USEM; ++i) {
    if (curr->usems[i]) {
      proc->usems[i] = usem_dup(curr->usems[i]);
    }
  }
  for (int i = 0; i < MAX_UFILE; ++i) {
    if (curr->files[i]) {
      proc->files[i] = fdup(curr->files[i]);
    }
  }
  proc->cwd = idup(curr->cwd);
  proc->parent = curr;
  curr->child_num += 1;
}

void proc_makezombie(proc_t *proc, int exitcode) {
  // Lab2-3: mark proc ZOMBIE and record exitcode, set children's parent to NULL
  // Lab2-5: close opened usem
  // Lab3-1: close opened files
  // Lab3-2: close cwd
  //TODO();
  proc->status = ZOMBIE;
  proc->exit_code = exitcode;
  for (proc_t *p = pcb; p < &pcb[PROC_NUM]; ++p) {
    if (p->parent == proc) {
      p->parent = NULL;
    }
  }
  for (int i = 0; i < MAX_USEM; ++i) {
    if (proc->usems[i]) {
      usem_close(proc->usems[i]);
      proc->usems[i] = NULL;
    }
  }
  for (int i = 0; i < MAX_UFILE; ++i) {
    if (proc->files[i]) {
      fclose(proc->files[i]);
      proc->files[i] = NULL;
    }
  }
  iclose(proc->cwd);
  proc->cwd = NULL;
  if (proc->parent) sem_v(&proc->parent->zombie_sem);
}

proc_t *proc_findzombie(proc_t *proc) {
  // Lab2-3: find a ZOMBIE whose parent is proc, return NULL if none
  //TODO();
  for (proc_t *p = pcb; p < &pcb[PROC_NUM]; ++p) {
    if (p->parent == proc && p->status == ZOMBIE) {
      return p;
    }
  }
  return NULL;
}

void proc_block() {
  // Lab2-4: mark curr proc BLOCKED, then int $0x81
  //TODO();
  curr->status = BLOCKED;
  INT(0x81);
}

int proc_allocusem(proc_t *proc) {
  // Lab2-5: find a free slot in proc->usems, return its index, or -1 if none
  //TODO();
  for (int i = 0; i < MAX_USEM; ++i) {
    if (proc->usems[i] == NULL) {
      return i;
    }
  }
  return -1;
}

usem_t *proc_getusem(proc_t *proc, int sem_id) {
  // Lab2-5: return proc->usems[sem_id], or NULL if sem_id out of bound
  //TODO();
  if (sem_id < 0 || sem_id >= MAX_USEM) return NULL;
  return proc->usems[sem_id];
}

int proc_allocfile(proc_t *proc) {
  // Lab3-1: find a free slot in proc->files, return its index, or -1 if none
  //TODO();
  for (int i = 0; i < MAX_UFILE; ++i) {
    if (proc->files[i] == NULL) {
      return i;
    }
  }
  return -1;
}

file_t *proc_getfile(proc_t *proc, int fd) {
  // Lab3-1: return proc->files[fd], or NULL if fd out of bound
  //TODO();
  if (fd < 0 || fd >= MAX_UFILE) return NULL;
  return proc->files[fd];
}

void schedule(Context *ctx) {
  // Lab2-1: save ctx to curr->ctx, then find a READY proc and run it
  //TODO();
  curr->ctx = ctx;
  for (proc_t *p = curr + 1; ; p++) {
    if (p == &pcb[PROC_NUM]) p = pcb;
    if (p->status == READY) proc_run(p);
  }
}
