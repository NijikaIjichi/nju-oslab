#ifndef __X86_CPU_H__
#define __X86_CPU_H__

#include "common.h"

/* 将CPU置入休眠状态直到下次中断到来 */
static inline void waitForInterrupt() {
	asm volatile("hlt");
}

/* 修改IDTR */
static inline void saveIdt(void *addr, uint32_t size) {
	static volatile uint16_t data[3];
	data[0] = size - 1;
	data[1] = (uint32_t)addr;
	data[2] = ((uint32_t)addr) >> 16;
	asm volatile("lidt (%0)" : : "r"(data));
}

/* 打开外部中断 */
static inline void enableInterrupt(void) {
	asm volatile("sti");
}

/* 关闭外部中断 */
static inline void disableInterrupt(void) {
	asm volatile("cli");
}

static inline int is_interrupt_on() {
	int efl;
	asm volatile (
		"pushf;"
		"popl %0;"
		: "=r"(efl)
	);
	return (efl & 0x200) != 0;
}

static inline uint32_t read_cr0()
{
	uint32_t val;
	asm volatile("movl %%cr0, %0"
				 : "=r"(val));
	return val;
}

static inline void write_cr0(uint32_t cr0)
{
	asm volatile("movl %0, %%cr0"
				 :: "r"(cr0));
}

static inline uint32_t read_cr3()
{
	uint32_t val;
	asm volatile("movl %%cr3, %0"
				 : "=r"(val));
	return val;
}

static inline void write_cr3(uint32_t cr3)
{
	asm volatile("movl %0, %%cr3"
				 :: "r"(cr3));
}

static inline uint32_t read_cr2()
{
	uint32_t val;
	asm volatile("movl %%cr2, %0"
				 : "=r"(val));
	return val;
}

void push_off();
void pop_off();

#define NR_IRQ    256

#define INT(x) asm volatile ("int $" #x)

#endif
