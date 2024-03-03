#include "x86/irq.h"
#include "x86/memory.h"

.globl irq0; irq0: push $0; push $0; jmp trap;
.globl irq1; irq1: push $0; push $1; jmp trap;
.globl irq2; irq2: push $0; push $2; jmp trap;
.globl irq3; irq3: push $0; push $3; jmp trap;
.globl irq4; irq4: push $0; push $4; jmp trap;
.globl irq5; irq5: push $0; push $5; jmp trap;
.globl irq6; irq6: push $0; push $6; jmp trap;
.globl irq7; irq7: push $0; push $7; jmp trap;
.globl irq8; irq8: ; push $8; jmp trap;
.globl irq9; irq9: push $0; push $9; jmp trap;
.globl irq10; irq10: ; push $10; jmp trap;
.globl irq11; irq11: ; push $11; jmp trap;
.globl irq12; irq12: ; push $12; jmp trap;
.globl irq13; irq13: ; push $13; jmp trap;
.globl irq14; irq14: ; push $14; jmp trap;
.globl irq15; irq15: push $0; push $15; jmp trap;
.globl irq16; irq16: push $0; push $16; jmp trap;
.globl irq32; irq32: push $0; push $32; jmp trap;
.globl irq33; irq33: push $0; push $33; jmp trap;
.globl irq34; irq34: push $0; push $34; jmp trap;
.globl irq35; irq35: push $0; push $35; jmp trap;
.globl irq36; irq36: push $0; push $36; jmp trap;
.globl irq37; irq37: push $0; push $37; jmp trap;
.globl irq38; irq38: push $0; push $38; jmp trap;
.globl irq39; irq39: push $0; push $39; jmp trap;
.globl irq40; irq40: push $0; push $40; jmp trap;
.globl irq41; irq41: push $0; push $41; jmp trap;
.globl irq42; irq42: push $0; push $42; jmp trap;
.globl irq43; irq43: push $0; push $43; jmp trap;
.globl irq44; irq44: push $0; push $44; jmp trap;
.globl irq45; irq45: push $0; push $45; jmp trap;
.globl irq46; irq46: push $0; push $46; jmp trap;
.globl irq47; irq47: push $0; push $47; jmp trap;
.globl irq128; irq128: push $0; push $128; jmp trap;
.globl irq129; irq129: push $0; push $129; jmp trap; // TODO
.globl irqall; irqall: push $0; push $-1;  jmp trap;

trap:
  pushl %eax
  pushl %ebx
  pushl %ecx
  pushl %edx
  pushl %esi
  pushl %edi
  pushl %ebp
  movw  %ds, %ax
  pushl %eax
  movw  $KSEL(SEG_KDATA), %ax
  movw  %ax, %ds
  movw  %ax, %es
  pushl %esp                  # esp is treated as a parameter
  call  irq_handle            # should never return
.L0:
  jmp   .L0

.globl irq_iret
irq_iret:
  movl 4(%esp), %eax
  movl %eax, %esp
  popl %eax
  movw %ax, %ds
  movw %ax, %es
  popl %ebp
  popl %edi
  popl %esi
  popl %edx
  popl %ecx
  popl %ebx
  popl %eax
  addl $8, %esp               # skip #irq and error code
  iret
