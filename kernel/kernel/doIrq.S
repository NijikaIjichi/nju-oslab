/*
otherwise need to reassign esp0 of tss in task switching for each user process
note that for conforming code OR same PRL, no need to switch task
that is TSS doesn't work
1. Trap Gate, IF won't be set
2. Interrupt Gate, IF is set automatically
3. System Gate
*/
#include <x86/irq.h>
#include <x86/memory.h>

.code32

.global irqEmpty
irqEmpty:
	pushl $0 // push dummy error code
	pushl $-1 // push interruption number into kernel
	jmp asmDoIrq

.global irqErrorCode
irqErrorCode:
	pushl $-1 // push interruption number into kernel
	jmp asmDoIrq

#define TABLE_ERR_NUM(name, id) \
	.global name; name: pushl $id; jmp asmDoIrq;

IRQ_ERROR_CODE(TABLE_ERR_NUM)

#define TABLE_HARDWARE(name, id) \
	.global name; name: pushl $0; pushl $id; jmp asmDoIrq;

IRQ_HARDWARE(TABLE_HARDWARE)

.global irqSyscall
irqSyscall:
	pushl $0 // push dummy error code
	pushl $0x80 // push interruption number into kernel stack
	jmp asmDoIrq

.global irq_yield
irq_yield:
	pushl $0
	pushl $0x81
	jmp asmDoIrq

.extern irqHandle //defined in irq_handle.c

.global asmDoIrq
asmDoIrq:
	pushal // push process state into kernel stack
	movw %ds, %ax
	pushl %eax
	movw $KSEL(SEG_KDATA), %ax
	movw %ax, %ds
	movw %ax, %es
	pushl $0 // hole for cr3
	pushl %esp //esp is treated as a parameter
	call irqHandle
	movl %eax, %esp
	addl $4, %esp // cr3 has been already loaded
	popl %eax
	movw %ax, %ds
	movw %ax, %es
	popal
	addl $4, %esp //interrupt number is on top of kernel stack
	addl $4, %esp //error code is on top of kernel stack
	iret
