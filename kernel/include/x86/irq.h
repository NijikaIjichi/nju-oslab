#ifndef __IRQ_H__
#define __IRQ_H__

#define IRQ_ERROR_CODE(_) \
  _(irqDoubleFault, 0x8) \
  _(irqInvalidTSS, 0xa) \
  _(irqSegNotPresent, 0xb) \
  _(irqStackSegFault, 0xc) \
  _(irqGProtectFault, 0xd) \
  _(irqPageFault, 0xe) \
  _(irqAlignCheck, 0x11) \
  _(irqSecException, 0x1e) \

#define IRQ_HARDWARE(_) \
  _(irq_timer, 0x20) \
  _(irqKeyboard, 0x21) \
  _(irq_0x22, 0x22) \
  _(irq_0x23, 0x23) \
  _(irq_0x24, 0x24) \
  _(irq_0x25, 0x25) \
  _(irq_0x26, 0x26) \
  _(irq_0x27, 0x27) \
  _(irq_0x28, 0x28) \
  _(irq_0x29, 0x29) \
  _(irq_0x2a, 0x2a) \
  _(irq_0x2b, 0x2b) \
  _(irq_0x2c, 0x2c) \
  _(irq_0x2d, 0x2d) \
  _(irq_0x2e, 0x2e) \
  _(irq_0x2f, 0x2f)

#ifndef __ASSEMBLER__
/* 中断处理相关函数 */
void initIdt(void);
void initIntr(void);
void initTimer(void);

#endif // __ASSEMBLER__
#endif // __IRQ_H__
