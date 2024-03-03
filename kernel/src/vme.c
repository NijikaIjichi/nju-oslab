#include "klib.h"
#include "vme.h"
#include "proc.h"

static TSS32 tss;

void init_gdt() {
  static SegDesc gdt[NR_SEG];
  gdt[SEG_KCODE] = SEG32(STA_X | STA_R,   0,     0xffffffff, DPL_KERN);
  gdt[SEG_KDATA] = SEG32(STA_W,           0,     0xffffffff, DPL_KERN);
  gdt[SEG_UCODE] = SEG32(STA_X | STA_R,   0,     0xffffffff, DPL_USER);
  gdt[SEG_UDATA] = SEG32(STA_W,           0,     0xffffffff, DPL_USER);
  gdt[SEG_TSS]   = SEG16(STS_T32A,     &tss,  sizeof(tss)-1, DPL_KERN);
  set_gdt(gdt, sizeof(gdt[0]) * NR_SEG);
  set_tr(KSEL(SEG_TSS));
}

void set_tss(uint32_t ss0, uint32_t esp0) {
  tss.ss0 = ss0;
  tss.esp0 = esp0;
}

static PD kpd;
static PT kpt[PHY_MEM / PT_SIZE] __attribute__((used));

static void *heap; // TODO: add at Lab1-4

void init_page() {
  extern char end;
  panic_on((size_t)(&end) >= KER_MEM - PGSIZE, "Kernel too big (MLE)");
  static_assert(sizeof(PTE) == 4, "PTE must be 4 bytes");
  static_assert(sizeof(PDE) == 4, "PDE must be 4 bytes");
  static_assert(sizeof(PT) == PGSIZE, "PT must be one page");
  static_assert(sizeof(PD) == PGSIZE, "PD must be one page");
  // Lab1-4: init kpd and kpt, identity mapping of [0 (or 4096), PHY_MEM)
  //TODO();
  for (int i = 0; i < PHY_MEM / PT_SIZE; ++i) {
    kpd.pde[i].val = MAKE_PDE(&kpt[i], 1);
    for (int j = 0; j < NR_PTE; ++j) {
      kpt[i].pte[j].val = MAKE_PTE((i << DIR_SHIFT) | (j << TBL_SHIFT), 1);
    }
  }
  kpt[0].pte[0].val = 0;
  set_cr3(&kpd);
  set_cr0(get_cr0() | CR0_PG);
  // Lab1-4: init free memory at [KER_MEM, PHY_MEM), a heap for kernel
  //TODO();
  heap = (void*)KER_MEM;
}

void *kalloc() {
  // Lab1-4: alloc a page from kernel heap, abort when heap empty
  //TODO();
  void *old_heap = heap;
  assert((size_t)heap < PHY_MEM);
  heap = (void*)((size_t)heap + PGSIZE);
  memset(old_heap, 0, PGSIZE);
  return old_heap;
}

void kfree(void *ptr) {
  // Lab1-4: free a page to kernel heap
  // you can just do nothing :)
  //TODO();
}

PD *vm_alloc() {
  // Lab1-4: alloc a new pgdir, map memory under PHY_MEM identityly
  //TODO();
  PD *pgdir = kalloc();
  for (int i = 0; i < PHY_MEM / PT_SIZE; ++i) {
    pgdir->pde[i].val = MAKE_PDE(&kpt[i], PTE_W);
  }
  return pgdir;
}

void vm_teardown(PD *pgdir) {
  // Lab1-4: free all pages mapping above PHY_MEM in pgdir, then free itself
  // you can just do nothing :)
  //TODO();
}

PD *vm_curr() {
  return (PD*)PAGE_DOWN(get_cr3());
}

PTE *vm_walkpte(PD *pgdir, size_t va, int prot) {
  // Lab1-4: return the pointer of PTE which match va
  // if not exist (PDE of va is empty) and prot&1, alloc PT and fill the PDE
  // if not exist (PDE of va is empty) and !(prot&1), return NULL
  // remember to let pde's prot |= prot, but not pte
  //TODO();
  assert((prot & ~7) == 0);
  PDE *pde = &pgdir->pde[ADDR2DIR(va)];
  if (pde->present == 0) {
    if (prot & PTE_P) pde->val = MAKE_PDE(kalloc(), prot);
    else return NULL;
  } else pde->val |= prot;
  PT *pt = PDE2PT(*pde);
  return &pt->pte[ADDR2TBL(va)];
}

void *vm_walk(PD *pgdir, size_t va, int prot) {
  // Lab1-4: translate va to pa
  // if prot&1 and prot voilation ((pte->val & prot & 7) != prot), call vm_pgfault
  // if va is not mapped and !(prot&1), return NULL
  //TODO();
  PTE *pte = vm_walkpte(pgdir, va, prot);
  if ((prot & PTE_P) && (pte->val & prot & 7) != prot) {
    vm_pgfault(va, prot);
  } else if (pte->present == 0 && !(prot & PTE_P)) {
    return NULL;
  } else {
    return (void*)((size_t)PTE2PG(*pte) | ADDR2OFF(va));
  }
}

void vm_map(PD *pgdir, size_t va, size_t len, int prot) {
  // Lab1-4: map [PAGE_DOWN(va), PAGE_UP(va+len)) at pgdir, with prot
  // if have already mapped pages, just let pte->prot |= prot
  //TODO();
  assert(prot & PTE_P);
  assert((prot & ~7) == 0);
  size_t start = PAGE_DOWN(va);
  size_t end = PAGE_UP(va + len);
  assert(start >= PHY_MEM);
  assert(end >= start);
  for (size_t i = start; i < end; i += PGSIZE) {
    PTE *pte = vm_walkpte(pgdir, i, prot);
    if (pte->present == 0) pte->val = MAKE_PTE(kalloc(), prot);
    else pte->val |= prot;
  }
}

void vm_unmap(PD *pgdir, size_t va, size_t len) {
  // Lab1-4: unmap and free [va, va+len) at pgdir
  // you can just do nothing :)
  //TODO();
  //assert(ADDR2OFF(va) == 0);
  //assert(ADDR2OFF(len) == 0);
}

void vm_copycurr(PD *pgdir) {
  // Lab2-2: copy memory mapped in curr pd to pgdir
  //TODO();
  PD *curr = vm_curr();
  for (size_t va = PHY_MEM; va < USR_MEM; va += PGSIZE) {
    PTE *pte = vm_walkpte(curr, va, 0);
    if (pte && pte->present) {
      vm_map(pgdir, va, PGSIZE, pte->val & 7);
      memcpy(vm_walk(pgdir, va, 0), (void*)va, PGSIZE);
    }
  }
}

void vm_pgfault(size_t va, int errcode) {
  printf("pagefault @ 0x%p, errcode = %d\n", va, errcode);
  panic("pgfault");
}
