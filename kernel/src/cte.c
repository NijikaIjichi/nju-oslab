#include "klib.h"
#include "cte.h"
#include "vme.h"
#include "serial.h"
#include "timer.h"
#include "proc.h"

static GateDesc32 idt[NR_IRQ];

void irq0();
void irq1();
void irq2();
void irq3();
void irq4();
void irq5();
void irq6();
void irq7();
void irq8();
void irq9();
void irq10();
void irq11();
void irq12();
void irq13();
void irq14();
void irq15();
void irq16();
void irq32();
void irq33();
void irq34();
void irq35();
void irq36();
void irq37();
void irq38();
void irq39();
void irq40();
void irq41();
void irq42();
void irq43();
void irq44();
void irq45();
void irq46();
void irq47();
void irq128();
void irq129(); // TODO
void irqall();

#define PORT_PIC_MASTER 0x20
#define PORT_PIC_SLAVE  0xA0
#define IRQ_SLAVE       2

static void init_intr() {
  outb(PORT_PIC_MASTER, 0x11); // you don't need to understand this
  outb(PORT_PIC_SLAVE, 0x11); // I don't understand either :)
  outb(PORT_PIC_MASTER + 1, 32);
  outb(PORT_PIC_SLAVE + 1, 32 + 8);
  outb(PORT_PIC_MASTER + 1, 1 << 2);
  outb(PORT_PIC_SLAVE + 1, 2);
  outb(PORT_PIC_MASTER + 1, 0x3);
  outb(PORT_PIC_SLAVE + 1, 0x3);
}

void init_cte() {
  for (int i = 0; i < NR_IRQ; i ++) {
    idt[i]  = GATE32(STS_IG, KSEL(SEG_KCODE), irqall, DPL_KERN);
  }
  idt[0] = GATE32(STS_IG, KSEL(SEG_KCODE), irq0, DPL_KERN);
  idt[1] = GATE32(STS_IG, KSEL(SEG_KCODE), irq1, DPL_KERN);
  idt[2] = GATE32(STS_IG, KSEL(SEG_KCODE), irq2, DPL_KERN);
  idt[3] = GATE32(STS_IG, KSEL(SEG_KCODE), irq3, DPL_KERN);
  idt[4] = GATE32(STS_IG, KSEL(SEG_KCODE), irq4, DPL_KERN);
  idt[5] = GATE32(STS_IG, KSEL(SEG_KCODE), irq5, DPL_KERN);
  idt[6] = GATE32(STS_IG, KSEL(SEG_KCODE), irq6, DPL_KERN);
  idt[7] = GATE32(STS_IG, KSEL(SEG_KCODE), irq7, DPL_KERN);
  idt[8] = GATE32(STS_IG, KSEL(SEG_KCODE), irq8, DPL_KERN);
  idt[9] = GATE32(STS_IG, KSEL(SEG_KCODE), irq9, DPL_KERN);
  idt[10] = GATE32(STS_IG, KSEL(SEG_KCODE), irq10, DPL_KERN);
  idt[11] = GATE32(STS_IG, KSEL(SEG_KCODE), irq11, DPL_KERN);
  idt[12] = GATE32(STS_IG, KSEL(SEG_KCODE), irq12, DPL_KERN);
  idt[13] = GATE32(STS_IG, KSEL(SEG_KCODE), irq13, DPL_KERN);
  idt[14] = GATE32(STS_IG, KSEL(SEG_KCODE), irq14, DPL_KERN);
  idt[15] = GATE32(STS_IG, KSEL(SEG_KCODE), irq15, DPL_KERN);
  idt[16] = GATE32(STS_IG, KSEL(SEG_KCODE), irq16, DPL_KERN);
  idt[32] = GATE32(STS_IG, KSEL(SEG_KCODE), irq32, DPL_KERN);
  idt[33] = GATE32(STS_IG, KSEL(SEG_KCODE), irq33, DPL_KERN);
  idt[34] = GATE32(STS_IG, KSEL(SEG_KCODE), irq34, DPL_KERN);
  idt[35] = GATE32(STS_IG, KSEL(SEG_KCODE), irq35, DPL_KERN);
  idt[36] = GATE32(STS_IG, KSEL(SEG_KCODE), irq36, DPL_KERN);
  idt[37] = GATE32(STS_IG, KSEL(SEG_KCODE), irq37, DPL_KERN);
  idt[38] = GATE32(STS_IG, KSEL(SEG_KCODE), irq38, DPL_KERN);
  idt[39] = GATE32(STS_IG, KSEL(SEG_KCODE), irq39, DPL_KERN);
  idt[40] = GATE32(STS_IG, KSEL(SEG_KCODE), irq40, DPL_KERN);
  idt[41] = GATE32(STS_IG, KSEL(SEG_KCODE), irq41, DPL_KERN);
  idt[42] = GATE32(STS_IG, KSEL(SEG_KCODE), irq42, DPL_KERN);
  idt[43] = GATE32(STS_IG, KSEL(SEG_KCODE), irq43, DPL_KERN);
  idt[44] = GATE32(STS_IG, KSEL(SEG_KCODE), irq44, DPL_KERN);
  idt[45] = GATE32(STS_IG, KSEL(SEG_KCODE), irq45, DPL_KERN);
  idt[46] = GATE32(STS_IG, KSEL(SEG_KCODE), irq46, DPL_KERN);
  idt[47] = GATE32(STS_IG, KSEL(SEG_KCODE), irq47, DPL_KERN);
  idt[128] = GATE32(STS_IG, KSEL(SEG_KCODE), irq128, DPL_USER);
  idt[129] = GATE32(STS_IG, KSEL(SEG_KCODE), irq129, DPL_KERN);
  set_idt(idt, sizeof(idt));
  init_intr();
}

void irq_handle(Context *ctx) {
  if (ctx->irq <= 16) {
    // just ignore me now, usage is in Lab1-6
    exception_debug_handler(ctx);
  }
  switch (ctx->irq) {
  // TODO: Lab1-5 handle pagefault and syscall
  // TODO: Lab1-7 handle serial and timer
  case EX_PF: vm_pgfault(get_cr2(), ctx->errcode); break;
  case T_IRQ0 + IRQ_TIMER: timer_handle(); break;
  case T_IRQ0 + IRQ_COM1: serial_handle(); break;
  case EX_SYSCALL: do_syscall(ctx); break;
  case 129: schedule(ctx); break;
  default: assert(ctx->irq >= T_IRQ0 && ctx->irq < T_IRQ0 + NR_INTR);
  }
  irq_iret(ctx);
}
