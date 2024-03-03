#include "x86.h"
#include "device.h"
#include "sys.h"
#include "proc.h"
#include "file.h"

extern int displayRow;
extern int displayCol;

void GProtectFaultHandle(TrapFrame *tf);
void page_fault_handle(TrapFrame *tf);
void KeyboardHandle(TrapFrame *tf);
void syscallHandle(TrapFrame *tf);

TrapFrame *schedule(TrapFrame *tf);

volatile int tick = 0;

static inline void stack_switch_call(void *sp, void *entry, uint32_t arg) {
  asm volatile (
    "movl %0, %%esp; movl %2, 4(%0); jmp *%1" : : "b"((uint32_t)sp - 8), "d"(entry), "a"(arg)
  );
}

TrapFrame *irqHandle(TrapFrame *tf) {
  extern proc_t *curr;
  assert((void*)tf < curr->kstack && (uint32_t)(tf) > (uint32_t)(curr->kstack) - PAGE_SIZE);
  switch(tf->irq) {
    case 0xd: GProtectFaultHandle(tf); break;
    case 0xe: page_fault_handle(tf); break;
    case 0x20: ++tick; wakeup((void*)&tick); yield(); break;
    case 0x21: KeyboardHandle(tf); break;
    case 0x80: syscallHandle(tf); break;
    case 0x81: tf = schedule(tf); break;
    default: assert((tf->irq & 0xf0) == 0x20);
  }
  if (curr && curr->killed) {
    tf->GPR1 = -1;
    stack_switch_call(tf, sys_exit, (uint32_t)tf);
  }
  return tf;
}

void GProtectFaultHandle(TrapFrame *tf) {
  assert(0);
}

void page_fault_handle(TrapFrame *tf) {
  printk("page fault @ 0x%d, eip = 0x%d.\n", read_cr2(), tf->eip);
  assert(0);
}

void KeyboardHandle(TrapFrame *tf){
  char ch = getChar(getKeyCode());
  if (ch == '\b') {
    if (displayCol > 0 && key_buf_pop_back() != 0) {
      putch(ch);
    }
  } else if (ch != 0) {
    key_buf_push_back(ch);
    putch(ch);
    wakeup(&key_buf_lock);
  }
}

void syscallHandle(TrapFrame *tf) {
  switch(tf->eax) {
    case SYS_write: sys_write(tf); break;
    case SYS_read: sys_read(tf); break;
    case SYS_yield: yield(); break;
    case SYS_fork: sys_fork(tf); break;
    case SYS_sleep: sys_sleep(tf); break;
    case SYS_exit: sys_exit(tf); break;
    case SYS_wait: sys_wait(tf); break;
    case SYS_exec: sys_exec(tf); break;
    case SYS_open: sys_open(tf); break;
    case SYS_dup: sys_dup(tf); break;
    case SYS_pipe: sys_pipe(tf); break;
    case SYS_close: sys_close(tf); break;
    case SYS_lseek: sys_lseek(tf); break;
    case SYS_kill: sys_kill(tf); break;
    case SYS_mkdir: sys_mkdir(tf); break;
    case SYS_chdir: sys_chdir(tf); break;
    case SYS_unlink: sys_unlink(tf); break;
    case SYS_fstat: sys_fstat(tf); break;
    case SYS_brk: sys_brk(tf); break;
    case SYS_getpid: sys_getpid(tf); break;
    default: tf->eax = -1;
  }
}
