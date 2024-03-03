#ifndef __X86_CPU_H__
#define __X86_CPU_H__

#include <stdint.h>
#include <stdbool.h>

// EFLAGS register
#define FL_IF          0x00000200  // Interrupt Enable

static inline uint8_t inb(int port) {
  uint8_t data;
  asm volatile ("inb %1, %0" : "=a"(data) : "d"((uint16_t)port));
  return data;
}

static inline uint16_t inw(int port) {
  uint16_t data;
  asm volatile ("inw %1, %0" : "=a"(data) : "d"((uint16_t)port));
  return data;
}

static inline uint32_t inl(int port) {
  uint32_t data;
  asm volatile ("inl %1, %0" : "=a"(data) : "d"((uint16_t)port));
  return data;
}

static inline void outb(int port, uint8_t data) {
  asm volatile ("outb %%al, %%dx" : : "a"(data), "d"((uint16_t)port));
}

static inline void outw(int port, uint16_t data) {
  asm volatile ("outw %%ax, %%dx" : : "a"(data), "d"((uint16_t)port));
}

static inline void outl(int port, uint32_t data) {
  asm volatile ("outl %%eax, %%dx" : : "a"(data), "d"((uint16_t)port));
}

static inline void cli() {
  asm volatile ("cli");
}

static inline void sti() {
  asm volatile ("sti");
}

static inline void hlt() {
  asm volatile ("hlt");
}

static inline void pause() {
  asm volatile ("pause");
}

static inline uint32_t get_efl() {
  volatile uintptr_t efl;
  asm volatile ("pushf; pop %0": "=r"(efl));
  return efl;
}

static inline bool ienabled() {
  return (get_efl() & FL_IF) != 0;
}

static inline uintptr_t get_cr0(void) {
  volatile uintptr_t val;
  asm volatile ("mov %%cr0, %0" : "=r"(val));
  return val;
}

static inline void set_cr0(uintptr_t cr0) {
  asm volatile ("mov %0, %%cr0" : : "r"(cr0));
}

static inline void set_idt(void *idt, int size) {
  static volatile struct {
    int16_t size;
    void *idt;
  } __attribute__((packed)) data;
  data.size = size;
  data.idt = idt;
  asm volatile ("lidt (%0)" : : "r"(&data));
}

static inline void set_gdt(void *gdt, int size) {
  static volatile struct {
    int16_t size;
    void *gdt;
  } __attribute__((packed)) data;
  data.size = size;
  data.gdt = gdt;
  asm volatile ("lgdt (%0)" : : "r"(&data));
}

static inline void set_tr(int selector) {
  asm volatile ("ltr %0" : : "r"((uint16_t)selector));
}

static inline uintptr_t get_cr2() {
  volatile uintptr_t val;
  asm volatile ("mov %%cr2, %0" : "=r"(val));
  return val;
}

static inline uintptr_t get_cr3() {
  volatile uintptr_t val;
  asm volatile ("mov %%cr3, %0" : "=r"(val));
  return val;
}

static inline void set_cr3(void *pdir) {
  asm volatile ("mov %0, %%cr3" : : "r"(pdir));
}

static inline void flush_tlb() {
  set_cr3((void*)get_cr3());
}

static inline int xchg(int *addr, int newval) {
  int result;
  asm volatile ("lock xchg %0, %1":
    "+m"(*addr), "=a"(result) : "1"(newval) : "cc", "memory");
  return result;
}

static inline void stack_switch_call(void *sp, void *entry, uintptr_t arg) {
  asm volatile (
    "movl %0, %%esp; movl %2, 4(%0); jmp *%1" : : "b"((uintptr_t)sp - 8), "d"(entry), "a"(arg)
  );
}

#endif
