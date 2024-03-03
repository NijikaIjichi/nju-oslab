#include "boot.h"

#define SECTSIZE 512
#define PH_LOAD 1  //Type为LOAD的段才可以被加载

void *load_memcpy(void *dst,void *src,unsigned len){
	int i = 0;
	for(i = 0; i < len; i++){
		*((unsigned char *)(dst+i)) = *((unsigned char *)(src+i));
	}
	return dst;
}

void *load_memset(void *dst,unsigned char ch,unsigned len){
	int i = 0;
	for(i = 0; i < len; i++){
		*((unsigned char *)(dst+i)) = ch;
	}
	return dst;
}

void bootMain(void) {
	int i = 0;
	int phoff = 0x34;
	int phnum = 0;
	unsigned int elf = 0x200000;
	void (*kMainEntry)(void);
	kMainEntry = (void(*)(void))0x0;
	struct ProgramHeader *thisph = 0x0;

	for (i = 0; i < 200; i++) {
		readSect((void*)(elf + i*512), 1+i);
	}

	kMainEntry = (void(*)(void))((struct ELFHeader *)elf)->entry;
	phoff = ((struct ELFHeader *)elf)->phoff;
	phnum = ((struct ELFHeader *)elf)->phnum;

	for(i = 0; i < phnum; i++){
		thisph = ((struct ProgramHeader *)(elf + phoff));
		if(thisph->type == PH_LOAD){
			load_memcpy((void *)thisph->vaddr, ((void *)elf+thisph->off), thisph->filesz);
			load_memset((void *)thisph->vaddr+thisph->filesz, 0, thisph->memsz-thisph->filesz);
		}
	}
	
	kMainEntry();
}

void waitDisk(void) { // waiting for disk
	while((inByte(0x1F7) & 0xC0) != 0x40);
}

void readSect(void *dst, int offset) { // reading a sector of disk
	int i;
	waitDisk();
	outByte(0x1F2, 1);
	outByte(0x1F3, offset);
	outByte(0x1F4, offset >> 8);
	outByte(0x1F5, offset >> 16);
	outByte(0x1F6, (offset >> 24) | 0xE0);
	outByte(0x1F7, 0x20);

	waitDisk();
	for (i = 0; i < SECTSIZE / 4; i ++) {
		((int *)dst)[i] = inLong(0x1F0);
	}
}
