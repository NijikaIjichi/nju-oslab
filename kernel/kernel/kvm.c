#include "x86.h"
#include "device.h"
#include "lib.h"

SegDesc gdt[NR_SEGMENTS];       // the new GDT, NR_SEGMENTS=7, defined in x86/memory.h
TSS tss;

//init GDT and LDT
void initSeg() { // setup kernel segements
  gdt[SEG_KCODE] = SEG(STA_X | STA_R, 0,       0xffffffff, DPL_KERN);
  gdt[SEG_KDATA] = SEG(STA_W,         0,       0xffffffff, DPL_KERN);
  gdt[SEG_UCODE] = SEG(STA_X | STA_R, 0,       0xffffffff, DPL_USER);
  gdt[SEG_UDATA] = SEG(STA_W,         0,       0xffffffff, DPL_USER);
  gdt[SEG_TSS] = SEG16(STS_T32A,      &tss, sizeof(TSS)-1, DPL_KERN);
  gdt[SEG_TSS].s = 0;
  setGdt(gdt, sizeof(gdt)); // gdt is set in bootloader, here reset gdt in kernel

  /*
   * 初始化TSS
   */
  tss.esp0 = 0x1ffffc;
  tss.ss0 = KSEL(SEG_KDATA);
  asm volatile("ltr %%ax":: "a" (KSEL(SEG_TSS)));

  /*设置正确的段寄存器*/
  asm volatile("movw %%ax,%%ds":: "a" (KSEL(SEG_KDATA)));
  asm volatile("movw %%ax,%%ss":: "a" (KSEL(SEG_KDATA)));

  lLdt(0);

}

PDE kpdir[NR_PDE] align_to_page;				// kernel page directory
PTE kptable[PHY_MEM / PAGE_SIZE] align_to_page; // kernel page tables

typedef struct page {
  struct page *next;
} page_t;

page_t *free_pages;

void init_vm() {
  for (uint32_t i = 0; i < PHY_MEM / PT_SIZE; ++i) {
    kpdir[i].val = MAKE_PDE(&kptable[i * NR_PTE], 0);
    for (uint32_t j = 0; j < NR_PTE; ++j) {
      kptable[i * NR_PTE + j].val = MAKE_PTE((i * NR_PTE + j) * PAGE_SIZE, PTE_W);
    }
  }
  write_cr3((uint32_t)&kpdir);
  write_cr0(read_cr0() | CR0_PG);
  uint32_t addr;
  for (addr = KMEM; addr < PHY_MEM - KMEM; addr += PAGE_SIZE) {
    ((page_t*)addr)->next = (page_t*)(addr + PAGE_SIZE);
  }
  ((page_t*)addr)->next = NULL;
  free_pages = (page_t*)KMEM;
  for (uint32_t addr = KMEM; addr < PHY_MEM; addr += PAGE_SIZE) {
    kptable[addr / PAGE_SIZE].present = 0;
  }
}

void *kalloc() {
  void *m = NULL;
  push_off();
  if (free_pages) {
    assert(kptable[((uint32_t)free_pages) / PAGE_SIZE].present == 0);
    kptable[((uint32_t)free_pages) / PAGE_SIZE].present = 1;
    m = free_pages;
    free_pages = free_pages->next;
  } else assert(0);
  pop_off();
  if (m) memset(m, 1, PAGE_SIZE);
  return m;
}

void kfree(void *m) {
  assert((((uint32_t)m) & PAGE_MASK) == 0);
  memset(m, 5, PAGE_SIZE);
  push_off();
  ((page_t*)m)->next = free_pages;
  free_pages = m;
  kptable[((uint32_t)free_pages) / PAGE_SIZE].present = 0;
  pop_off();
}

PDE *uvm_protect() {
  PDE *pgdir = kalloc();
  memset(pgdir, 0, NR_PDE * sizeof(PDE));
  memcpy(pgdir, kpdir, (PHY_MEM / PT_SIZE) * sizeof(PDE));
  return pgdir;
}

void uvm_copy(PDE *dst, PDE *src) {
  for (int i = PHY_MEM / PT_SIZE; i < NR_PDE; ++i) {
    if (src[i].present) {
      PTE *src_pte = (PTE*)(src[i].page_frame << 12), *dst_pte = kalloc();
      memset(dst_pte, 0, PAGE_SIZE);
      dst[i].val = MAKE_PDE(dst_pte, FLAG(src[i]));
      for (int j = 0; j < NR_PTE; ++j) {
        if (src_pte[j].present) {
          void *src_m = (void*)(src_pte[j].page_frame << 12), *dst_m = kalloc();
          memcpy(dst_m, src_m, PAGE_SIZE);
          dst_pte[j].val = MAKE_PTE(dst_m, FLAG(src_pte[j]));
        }
      }
    }
  }
}

void uvm_alloc(PDE *pgdir, uint32_t uva, uint32_t len, int prot) {
  assert(uva >= PHY_MEM);
  for (uint32_t i = 0; i < PAGE_UP(len); i += PAGE_SIZE, uva += PAGE_SIZE) {
    if (!pgdir[ADDR2DIR(uva)].present) {
      void *pg = kalloc();
      memset(pg, 0, PAGE_SIZE);
      pgdir[ADDR2DIR(uva)].val = MAKE_PDE(pg, prot);
    }
    pgdir[ADDR2DIR(uva)].val |= prot;
    PTE *pgtbl = (void*)(pgdir[ADDR2DIR(uva)].page_frame << 12);
    assert(!pgtbl[ADDR2TBL(uva)].present);
    pgtbl[ADDR2TBL(uva)].val = MAKE_PTE(kalloc(), prot);
  }
}

void uvm_dealloc(PDE *pgdir, uint32_t uva, uint32_t len) {
  assert(uva >= PHY_MEM);
  for (uint32_t i = 0; i < PAGE_UP(len); i += PAGE_SIZE, uva += PAGE_SIZE) {
    if (pgdir[ADDR2DIR(uva)].present) {
      PTE *pgtbl = (void*)(pgdir[ADDR2DIR(uva)].page_frame << 12);
      if (pgtbl[ADDR2TBL(uva)].present) {
        void *m = (void*)(pgtbl[ADDR2TBL(uva)].page_frame << 12);
        pgtbl[ADDR2TBL(uva)].val = 0;
        kfree(m);
      }
    }
  }
}

void uvm_free(PDE *pgdir) {
  for (int i = PHY_MEM / PT_SIZE; i < NR_PDE; ++i) {
    if (pgdir[i].present) {
      void *m = (void*)(pgdir[i].page_frame << 12);
      pgdir[i].val = 0;
      kfree(m);
    }
  }
  kfree(pgdir);
}

void *uvm_walk(PDE *pgdir, uint32_t uva) {
  assert(pgdir[ADDR2DIR(uva)].present);
  PTE *pgtbl = (void*)(pgdir[ADDR2DIR(uva)].page_frame << 12);
  assert(pgtbl[ADDR2TBL(uva)].present);
  return (void*)((pgtbl[ADDR2TBL(uva)].page_frame << 12) | ADDR2OFF(uva));
}
