#ifndef __X86_IO_H__
#define __X86_IO_H__

static inline int inLong(short port) {
	int data;
	asm volatile("in %1, %0" : "=a" (data) : "d" (port));
	return data;
}

/* 读I/O端口 */
static inline uint8_t inByte(uint16_t port) {
	uint8_t data;
	asm volatile("in %1, %0" : "=a"(data) : "d"(port));
	return data;
}

/* 写I/O端口 */
static inline void outByte(uint16_t port, int8_t data) {
	asm volatile("out %%al, %%dx" : : "a"(data), "d"(port));
}

static inline void outLong(uint16_t port, int32_t data) {
	asm volatile("out %%eax, %%dx" : : "a"(data), "d"(port));
}

#endif
