#include "x86.h"
#include "device.h"

#define INTERRUPT_GATE_32   0xE
#define TRAP_GATE_32        0xF

/* IDT表的内容 */
struct GateDescriptor idt[NR_IRQ]; // NR_IRQ=256, defined in x86/cpu.h

/* 初始化一个中断门(interrupt gate) */
static void setIntr(struct GateDescriptor *ptr, uint32_t selector, uint32_t offset, uint32_t dpl) {
  ptr->offset_15_0 = offset & 0xffff;
  ptr->offset_31_16 = (offset >> 16) & 0xffff;
  ptr->pad0 = 0;
  ptr->present = 1;
  ptr->privilege_level = dpl;
  ptr->segment = KSEL(selector);
  ptr->system = 0;
  ptr->type = STS_IG32;
}

/* 初始化一个陷阱门(trap gate) */
static void setTrap(struct GateDescriptor *ptr, uint32_t selector, uint32_t offset, uint32_t dpl) {
  ptr->offset_15_0 = offset & 0xffff;
  ptr->offset_31_16 = (offset >> 16) & 0xffff;
  ptr->pad0 = 0;
  ptr->present = 1;
  ptr->privilege_level = dpl;
  ptr->segment = KSEL(selector);
  ptr->system = 0;
  ptr->type = STS_TG32;
}

/* 声明函数，这些函数在汇编代码里定义 */
void irqEmpty();
void irqErrorCode();

#define DECLARE_IRQ(name, err) void name();

IRQ_ERROR_CODE(DECLARE_IRQ)
IRQ_HARDWARE(DECLARE_IRQ);

void irqSyscall();
void irq_yield();


//256 interrupt vector
void initIdt() {
  int i;
  /* 为了防止系统异常终止，所有irq都有处理函数(irqEmpty)。 */
  for (i = 0; i < NR_IRQ; i ++) {
    setTrap(idt + i, SEG_KCODE, (uint32_t)irqEmpty, DPL_KERN);
  }
  /*init your idt here 初始化 IDT 表, 为中断设置中断处理函数*/
#define SET_IRQ_INTR(name, id) setIntr(idt + id, SEG_KCODE, (uint32_t)name, DPL_KERN);
  IRQ_HARDWARE(SET_IRQ_INTR);

  /* Exceptions with error code */
#define SET_IRQ_TRAP(name, id) setTrap(idt + id, SEG_KCODE, (uint32_t)name, DPL_KERN);
  IRQ_ERROR_CODE(SET_IRQ_TRAP)

  /* Exceptions with DPL = 3 */
  setTrap(idt + 0x80, SEG_KCODE, (uint32_t)irqSyscall, DPL_USER);

  setTrap(idt + 0x81, SEG_KCODE, (uint32_t)irq_yield, DPL_KERN);

  /* 写入IDT */
  saveIdt(idt, sizeof(idt));//use lidt
}
