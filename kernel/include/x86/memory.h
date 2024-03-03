#ifndef __X86_MEMORY_H__
#define __X86_MEMORY_H__

// CPU rings
#define DPL_KERN       0x0     // Kernel (ring 0)
#define DPL_USER       0x3     // User (ring 3)

// Application Segment type bits
#define STA_X          0x8     // Executable segment
#define STA_W          0x2     // Writeable (non-executable segments)
#define STA_R          0x2     // Readable (executable segments)

// System Segment type bits
#define STS_T32A       0x9     // Available 32-bit TSS
#define STS_IG         0xe     // 32/64-bit Interrupt Gate
#define STS_TG         0xf     // 32/64-bit Trap Gate

// Control Register flags
#define CR0_PE         0x00000001  // Protection Enable
#define CR0_PG         0x80000000  // Paging

// Page table/directory entry flags
#define PTE_P          0x001   // Present
#define PTE_W          0x002   // Writeable
#define PTE_U          0x004   // User

// GDT selectors
#define KSEL(seg)      (((seg) << 3) | DPL_KERN)
#define USEL(seg)      (((seg) << 3) | DPL_USER)

#define NR_SEG         6       // GDT size
#define SEG_KCODE      1       // Kernel code
#define SEG_KDATA      2       // Kernel data/stack
#define SEG_UCODE      3       // User code
#define SEG_UDATA      4       // User data/stack
#define SEG_TSS        5       // Global unique task state segement

#ifndef __ASSEMBLER__

#include <stdint.h>

// Segment Descriptor
typedef struct {
  uint32_t lim_15_0   : 16; // Low bits of segment limit
  uint32_t base_15_0  : 16; // Low bits of segment base address
  uint32_t base_23_16 :  8; // Middle bits of segment base address
  uint32_t type       :  4; // Segment type (see STS_ constants)
  uint32_t s          :  1; // 0 = system, 1 = application
  uint32_t dpl        :  2; // Descriptor Privilege Level
  uint32_t p          :  1; // Present
  uint32_t lim_19_16  :  4; // High bits of segment limit
  uint32_t avl        :  1; // Unused (available for software use)
  uint32_t l          :  1; // 64-bit segment
  uint32_t db         :  1; // 32-bit segment
  uint32_t g          :  1; // Granularity: limit scaled by 4K when set
  uint32_t base_31_24 :  8; // High bits of segment base address
} SegDesc;

// Task State Segment (TSS)
typedef struct {
  uint32_t link;     // Unused
  uint32_t esp0;     // Stack pointers and segment selectors
  uint32_t ss0;      //   after an increase in privilege level
  uint32_t padding[23];
} __attribute__((packed)) TSS32;

#define SEG16(type, base, lim, dpl) (SegDesc)        \
{ (lim) & 0xffff, (uintptr_t)(base) & 0xffff,        \
  ((uintptr_t)(base) >> 16) & 0xff, type, 0, dpl, 1, \
  (uintptr_t)(lim) >> 16, 0, 0, 1, 0, (uintptr_t)(base) >> 24 }

#define SEG32(type, base, lim, dpl) (SegDesc)         \
{ ((lim) >> 12) & 0xffff, (uintptr_t)(base) & 0xffff, \
  ((uintptr_t)(base) >> 16) & 0xff, type, 1, dpl, 1,  \
  (uintptr_t)(lim) >> 28, 0, 0, 1, 1, (uintptr_t)(base) >> 24 }


#define KER_MEM   0x00200000  // the max static memory of kernel
#define PHY_MEM   0x08000000  // QEMU has 128MB physical memory
#define USR_MEM   0xc0000000  // the memory top of user proc

#define PGSIZE    4096                           // page size in x86
#define PGMASK    (PGSIZE - 1)                   // page mask in x86
#define PGBITS    12                             // page bits in x86
#define NR_PDE    1024                           // number of PDE in PD
#define NR_PTE    1024                           // number of PTE in PT
#define PT_SIZE   ((NR_PTE) * (PGSIZE))          // size of address a PT can cover
#define PG_ALIGN  __attribute((aligned(PGSIZE))) // force data aligned with page

#define PAGE_DOWN(addr) (((uint32_t)(addr)) & (~PGMASK))
#define PAGE_UP(addr)   (((uint32_t)(addr) + PGMASK) & (~PGMASK))

#define DIR_MASK  0xffc00000
#define DIR_SHIFT 22
#define TBL_MASK  0x003ff000
#define TBL_SHIFT 12
#define OFF_MASK  0x00000fff

// calculate the PD-index/PT-index/offset of a address
#define ADDR2DIR(addr) (((uint32_t)(addr) & DIR_MASK) >> DIR_SHIFT)
#define ADDR2TBL(addr) ((((uint32_t)(addr)) & TBL_MASK) >> TBL_SHIFT)
#define ADDR2OFF(addr) (((uint32_t)(addr)) & OFF_MASK)

/* the 32bit Page Directory(first level page table) data structure */
typedef union PageDirectoryEntry {
  struct {
    uint32_t present : 1;
    uint32_t read_write : 1;
    uint32_t user_supervisor : 1;
    uint32_t page_write_through : 1;
    uint32_t page_cache_disable : 1;
    uint32_t accessed : 1;
    uint32_t pad0 : 6;
    uint32_t page_frame : 20;
  };
  uint32_t val;
} PDE;

typedef struct PageDirectory {
  PDE pde[NR_PDE] PG_ALIGN;
} PD;

/* the 32bit Page Table Entry(second level page table) data structure */
typedef union PageTableEntry {
  struct {
    uint32_t present : 1;
    uint32_t read_write : 1;
    uint32_t user_supervisor : 1;
    uint32_t page_write_through : 1;
    uint32_t page_cache_disable : 1;
    uint32_t accessed : 1;
    uint32_t dirty : 1;
    uint32_t pad0 : 1;
    uint32_t global : 1;
    uint32_t pad1 : 3;
    uint32_t page_frame : 20;
  };
  uint32_t val;
} PTE;

typedef struct PageTable {
  PTE pte[NR_PTE] PG_ALIGN;
} PT;

#define MAKE_PDE(addr, prot) (PAGE_DOWN(addr) | (prot) | (PTE_P))
#define MAKE_PTE(addr, prot) (PAGE_DOWN(addr) | (prot) | (PTE_P))

#define PDE2PT(pde) ((PT*)((pde).page_frame << PGBITS))
#define PTE2PG(pte) ((void*)((pte).page_frame << PGBITS))

#endif // __ASSEMBLER__
#endif // __X86_MEMORY_H__
