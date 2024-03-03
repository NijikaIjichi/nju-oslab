#ifndef __LOADER_H__
#define __LOADER_H__

#include <stdint.h>

uint32_t load_elf(struct PageDirectory *pgdir, const char *name);
uint32_t load_arg(struct PageDirectory *pgdir, char *const argv[]);
int load_user(struct PageDirectory *pgdir, struct Context *ctx, 
              const char *name, char *const argv[]);

#endif
