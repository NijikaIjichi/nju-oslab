#ifndef __LIB__
#define __LIB__

void memcpy(void *dst, const void *src, uint32_t size);
void memset(void *dst, int c, uint32_t size);
void strcpy(char *dst, const char *src);
int strcmp(const char *s1, const char *s2);
uint32_t strlen(const char *str);

void memset_uvm(PDE *pgdir, uint32_t dst, int c, uint32_t size);
void strcpy_uvm(PDE *pgdir, uint32_t dst, const char *src);


#endif
