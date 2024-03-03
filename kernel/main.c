#include "common.h"
#include "x86.h"
#include "device.h"
#include "proc.h"
#include "fs.h"

void kEntry(void) {
	// Interruption is disabled in bootloader
	initSerial();// initialize serial port
	
	// initialize idt
	initIdt();
	// iniialize 8259a
	initIntr();
	initTimer();
	// initialize gdt, tss
	initSeg();
	init_vm();
	// initialize vga device
	initVga();
	// initialize keyboard device
	initKeyTable();
	init_proc();
	init_fs();
	load_init_proc();
	init_device();

	for (;;) {
		yield();
		enableInterrupt();
		waitForInterrupt();
	}
}
