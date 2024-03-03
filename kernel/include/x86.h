#ifndef __X86_H__
#define __X86_H__

#include "x86/cpu.h"
#include "x86/memory.h"
#include "x86/io.h"
#include "x86/irq.h"

void initSeg(void);
void init_vm(void);
void init_proc(void);
void load_init_proc(void);

#endif
