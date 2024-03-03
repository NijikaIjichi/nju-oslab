#ifndef __X86_MEMORY_H__
#define __X86_MEMORY_H__

#define DPL_KERN                0
#define DPL_USER                3

// Application segment type bits
#define STA_X       0x8         // Executable segment
#define STA_W       0x2         // Writeable (non-executable segments)
#define STA_R       0x2         // Readable (executable segments)

// System segment type bits
#define STS_T32A    0x9         // Available 32-bit TSS
#define STS_IG32    0xE         // 32-bit Interrupt Gate
#define STS_TG32    0xF         // 32-bit Trap Gate

// GDT entries
#define NR_SEGMENTS      7           // GDT size
#define SEG_KCODE   1           // Kernel code
#define SEG_KDATA   2           // Kernel data/stack
#define SEG_UCODE   3           // User code
#define SEG_UDATA   4           // User data/stack
#define SEG_TSS     5           // Global unique task state segement

// Selectors
#define KSEL(desc) (((desc) << 3) | DPL_KERN)
#define USEL(desc) (((desc) << 3) | DPL_USER)

#define PAGE_SIZE 4096
#define NR_PDE 1024
#define NR_PTE 1024
#define PAGE_MASK (4096 - 1)
#define PT_SIZE ((NR_PTE) * (PAGE_SIZE))

#define ADDR2DIR(addr) (((uint32_t)(addr)) >> 22)
#define ADDR2TBL(addr) ((((uint32_t)(addr)) >> 12) & 0x3ff)
#define ADDR2OFF(addr) (((uint32_t)(addr)) & 0xfff)

/* force the data to be aligned with page boundary.
   statically defined page tables uses this feature. */
#define align_to_page __attribute((aligned(PAGE_SIZE)))

// Control Register flags
#define CR0_PE         0x00000001  // Protection Enable
#define CR0_PG         0x80000000  // Paging

// Page table/directory entry flags
#define PTE_P          0x001   // Present
#define PTE_W          0x002   // Writeable
#define PTE_U          0x004   // User

/* the maxinum loader size is 2MB */
#define KMEM (2 * 1024 * 1024)

/* NEMU has 128MB physical memory  */
#define PHY_MEM (128 * 1024 * 1024)

#define MAKE_PDE(addr, prot) ((((uint32_t)(addr)) & 0xfffff000) | (prot) | (PTE_P))
#define MAKE_PTE(addr, prot) ((((uint32_t)(addr)) & 0xfffff000) | (prot) | (PTE_P))

#define PAGE_DOWN(addr) (((uint32_t)addr) & 0xfffff000)
#define PAGE_UP(addr) (((uint32_t)addr + 0xfff) & 0xfffff000)

#ifndef __ASSEMBLER__
struct GateDescriptor {
	uint32_t offset_15_0      : 16;
	uint32_t segment          : 16;
	uint32_t pad0             : 8;
	uint32_t type             : 4;
	uint32_t system           : 1;
	uint32_t privilege_level  : 2;
	uint32_t present          : 1;
	uint32_t offset_31_16     : 16;
};

struct TrapFrame {
	uint32_t cr3, ds;
	uint32_t edi, esi, ebp, xxx, ebx, edx, ecx, eax;
	int32_t irq, errorcode;
	uint32_t eip, cs, eflags, esp, ss;
};

typedef struct TrapFrame TrapFrame;

/*
1. The number of bits in a bit field sets the limit to the range of values it can hold
2. Multiple adjacent bit fields are usually packed together (although this behavior is implementation-defined)

Refer: en.cppreference.com/w/cpp/language/bit_field
*/
struct SegDesc {
	uint32_t lim_15_0 : 16;  // Low bits of segment limit
	uint32_t base_15_0 : 16; // Low bits of segment base address
	uint32_t base_23_16 : 8; // Middle bits of segment base address
	uint32_t type : 4;       // Segment type (see STS_ constants)
	uint32_t s : 1;          // 0 = system, 1 = application
	uint32_t dpl : 2;        // Descriptor Privilege Level
	uint32_t p : 1;          // Present
	uint32_t lim_19_16 : 4;  // High bits of segment limit
	uint32_t avl : 1;        // Unused (available for software use)
	uint32_t rsv1 : 1;       // Reserved
	uint32_t db : 1;         // 0 = 16-bit segment, 1 = 32-bit segment
	uint32_t g : 1;          // Granularity: limit scaled by 4K when set
	uint32_t base_31_24 : 8; // High bits of segment base address
};
typedef struct SegDesc SegDesc;

#define SEG(type, base, lim, dpl) (SegDesc)                   \
{	((lim) >> 12) & 0xffff, (uint32_t)(base) & 0xffff,        \
	((uint32_t)(base) >> 16) & 0xff, type, 1, dpl, 1,         \
	(uint32_t)(lim) >> 28, 0, 0, 1, 1, (uint32_t)(base) >> 24 }

#define SEG16(type, base, lim, dpl) (SegDesc)                 \
{	(lim) & 0xffff, (uint32_t)(base) & 0xffff,                \
	((uint32_t)(base) >> 16) & 0xff, type, 0, dpl, 1,         \
	(uint32_t)(lim) >> 16, 0, 0, 1, 0, (uint32_t)(base) >> 24 }
	
// Task state segment format
struct TSS {
	uint32_t link;         // old ts selector
	uint32_t esp0;         // Ring 0 Stack pointer and segment selector
	uint32_t ss0;          // after an increase in privilege level
	union{
		struct{
			char dontcare[88];
		};
		struct{
			uint32_t esp1,ss1,esp2,ss2;
			uint32_t cr3, eip, eflags, eax, ecx, edx, ebx, esp, ebp, esi, edi;
			uint32_t es, cs, ss, ds, fs, gs, ldt;
		};
        };
};
typedef struct TSS TSS;

static inline void setGdt(SegDesc *gdt, uint32_t size) {
	volatile static uint16_t data[3];
	data[0] = size - 1;
	data[1] = (uint32_t)gdt;
	data[2] = (uint32_t)gdt >> 16;
	asm volatile("lgdt (%0)" : : "r"(data));
}

static inline void lLdt(uint16_t sel)
{
	asm volatile("lldt %0" :: "r"(sel));
}

/* the 32bit Page Directory(first level page table) data structure */
typedef union PageDirectoryEntry {
	struct
	{
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

/* the 32bit Page Table Entry(second level page table) data structure */
typedef union PageTableEntry {
	struct
	{
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

#define FLAG(p) ((p).val & 0x7)

void *kalloc();
void kfree(void *m);
PDE *uvm_protect();
void uvm_copy(PDE *dst, PDE *src);
void uvm_alloc(PDE *pgdir, uint32_t uva, uint32_t len, int prot);
void uvm_dealloc(PDE *pgdir, uint32_t uva, uint32_t len);
void uvm_free(PDE *pgdir);
void *uvm_walk(PDE *pgdir, uint32_t uva);

#endif // __ASSEMBLER__
#endif // __X86_MEMORY_H__
