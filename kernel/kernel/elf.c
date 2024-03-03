#define __TYPES_H__
#include <elf.h>
#include "x86.h"
#include "device.h"
#include "sys.h"
#include "proc.h"
#include "file.h"
#include "lib.h"
#include "fs.h"

#define MAX_ARGS_NUM 31

static uint32_t push_uvm(PDE *pgdir, uint32_t esp, uint32_t word) {
  esp -= 4;
  *(uint32_t*)uvm_walk(pgdir, esp) = word;
  return esp;
}

int load_elf(char *path, char *argv[], PDE **pgtbl, uint32_t *entry, uint32_t *ustack_top, uint32_t ustack_size) {
  uint32_t argc, argv_pos[MAX_ARGS_NUM];
  icache_t *ip = iopen(path, NULL, 0, 0, 0, NULL);
  if (ip == NULL) return -1;
  Elf32_Ehdr elf_head;
  iread(ip, &elf_head, sizeof(elf_head), 0);
  if (*(uint32_t*)(&elf_head) != 0x464c457f) {
    iput(ip);
    return -1;
  }
  PDE *upgdir = uvm_protect(), *curr = (PDE*)read_cr3();
  *entry = elf_head.e_entry;
  *pgtbl = upgdir;
  Elf32_Phdr phdr;
  push_off();
  write_cr3((uint32_t)upgdir);
  for (int i = 0; i < elf_head.e_phnum; ++i) {
    iread(ip, &phdr, sizeof(phdr), elf_head.e_phoff + i * sizeof(phdr));
    if (phdr.p_type == PT_LOAD) {
      uvm_alloc(upgdir, phdr.p_vaddr, phdr.p_memsz, PTE_U | ((phdr.p_flags & PF_W) ? PTE_W : 0)); 
      iread(ip, (void*)(phdr.p_vaddr), phdr.p_filesz, phdr.p_offset);
      memset((void*)(phdr.p_vaddr + phdr.p_filesz), 0, phdr.p_memsz - phdr.p_filesz); 
    }
  }
  write_cr3((uint32_t)curr);
  pop_off();
  uvm_alloc(upgdir, *ustack_top - ustack_size, ustack_size, PTE_U | PTE_W);
  for (argc = 0; argv[argc] && argc < MAX_ARGS_NUM; ++argc) {
    *ustack_top -= strlen(argv[argc]) + 1;
    strcpy_uvm(upgdir, *ustack_top, argv[argc]);
    argv_pos[argc] = *ustack_top;
  }
  *ustack_top -= *ustack_top % 4;
  *ustack_top = push_uvm(upgdir, *ustack_top, 0);
  for (int i = argc - 1; i >= 0; --i) {
    *ustack_top = push_uvm(upgdir, *ustack_top, argv_pos[i]);
  }
  *ustack_top = push_uvm(upgdir, *ustack_top, *ustack_top);
  *ustack_top = push_uvm(upgdir, *ustack_top, argc);
  *ustack_top = push_uvm(upgdir, *ustack_top, 0xffffffff);
  return 0;
}
