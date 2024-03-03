#ifndef __X86_IRQ_H__
#define __X86_IRQ_H__

// Interrupts and exceptions
#define T_IRQ0         32
#define IRQ_TIMER      0
#define IRQ_COM1       4
#define EX_DE          0
#define EX_UD          6
#define EX_NM          7
#define EX_DF          8
#define EX_TS          10
#define EX_NP          11
#define EX_SS          12
#define EX_GP          13
#define EX_PF          14
#define EX_MF          15
#define EX_SYSCALL     0x80

#define NR_IRQ         256     // IDT size
#define NR_INTR        16

#ifndef __ASSEMBLER__

#include <stdint.h>

// Gate descriptors for interrupts and traps
typedef struct {
  uint32_t off_15_0  : 16; // Low 16 bits of offset in segment
  uint32_t cs        : 16; // Code segment selector
  uint32_t args      :  5; // # args, 0 for interrupt/trap gates
  uint32_t rsv1      :  3; // Reserved(should be zero I guess)
  uint32_t type      :  4; // Type(STS_{TG,IG32,TG32})
  uint32_t s         :  1; // Must be 0 (system)
  uint32_t dpl       :  2; // Descriptor(meaning new) privilege level
  uint32_t p         :  1; // Present
  uint32_t off_31_16 : 16; // High bits of offset in segment
} GateDesc32;

#define GATE32(type, cs, entry, dpl) (GateDesc32)              \
  {  (uint32_t)(entry) & 0xffff, (cs), 0, 0, (type), 0, (dpl), \
  1, (uint32_t)(entry) >> 16 }

#define INT(x) asm volatile ("int $" #x)

#endif // __ASSEMBLER__
#endif // __X86_IRQ_H__
