#ifndef __VME_H__
#define __VME_H__

#include "klib.h"

void init_gdt();
void set_tss(uint32_t ss0, uint32_t esp0);

void init_page();
void *kalloc();
void kfree(void *ptr);

PD *vm_alloc();
void vm_teardown(PD *pgdir);
PD *vm_curr();
PTE *vm_walkpte(PD *pgdir, size_t va, int prot);
void *vm_walk(PD *pgdir, size_t va, int prot);
void vm_map(PD *pgdir, size_t va, size_t len, int prot);
void vm_unmap(PD *pgdir, size_t va, size_t len);
void vm_copycurr(PD *pgdir);
void vm_pgfault(size_t va, int errcode) __attribute__((noreturn));

#endif
