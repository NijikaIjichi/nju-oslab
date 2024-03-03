#ifndef __LIB_H__
#define __LIB_H__

#include <stddef.h>
#include <stdint.h>
#include <limits.h>
#include <stdbool.h>
#include <stdarg.h>

#define MIN(a, b) (((a) > (b)) ? (b) : (a))
#define MAX(a, b) (((a) < (b)) ? (b) : (a))
#define ABS(x)    (((x) < 0) ? -(x) : (x))

// stdio, printf is in klib/ulib, scanf is in ulib
int cprintf(void (*putstr)(const char*), const char *format, ...); // LEN LIMIT 512
int vcprintf(void (*putstr)(const char*), const char *format, va_list args);
int sprintf(char *buf, const char *format, ...);
int vsprintf(char *buf, const char *fmt, va_list args);

// stdlib
int atoi(const char *str);
char *itoa(int value, char *str, int base);
char *utoa(unsigned value, char *str, int base);
long strtol(const char *__restrict str, char **__restrict endptr, int base);
unsigned long strtoul(const char *__restrict str, char **__restrict endptr, int base);
int rand();
void srand(unsigned int seed);
#define RAND_MAX 2147483647

// string
void *memccpy(void *__restrict dst, const void *__restrict src, int endchar, size_t len);
void *memchr(const void *src, int c, size_t length);
int memcmp(const void *m1, const void *m2, size_t n);
void *memcpy(void *__restrict dst, const void *__restrict src, size_t len);
void *memmem(const void *haystack, size_t hs_len, const void *needle, size_t ne_len);
void *memmove(void *dst, const void *src, size_t length);
void *memrchr(const void *src, int c, size_t length);
void *memset(void *m, int c, size_t n);
char *strcat(char *__restrict s1, const char *__restrict s2);
char *strchr(const char *s1, int i);
int strcmp(const char *s1, const char *s2);
char *strcpy(char *dst, const char *src);
size_t strcspn(const char *s1, const char *s2);
size_t strlen(const char *str);
char *strncat(char *__restrict s1, const char *__restrict s2, size_t n);
int strncmp(const char *s1, const char *s2, size_t n);
char *strncpy(char *__restrict dst, const char *__restrict src, size_t count);
size_t strnlen(const char *str, size_t n);
char *strnstr(const char *haystack, const char *needle, size_t haystack_len);
char *strpbrk(const char *s1, const char *s2);
char *strrchr(const char *s, int i);
size_t strspn(const char *s1, const char *s2);
char *strstr(const char *hs, const char *ne);
char *strtok(char *__restrict s, const char *__restrict delim);

// ctype
#define isspace(c) ((c) == ' ' || (c) == '\t' || (c) == '\n')

// assert, abort is in klib/ulib
#define assert(cond) \
  ((cond) ? ((void)0) : abort(__FILE__, __LINE__, #cond))

#define panic_on(cond, info) \
  ((cond) ? (abort(__FILE__, __LINE__, info)) : ((void)0))

#define panic(info) \
  abort(__FILE__, __LINE__, info)

#define TODO() \
  panic("Implement me")

#ifndef static_assert
#define static_assert(a, b) do { switch (0) case 0: case (a): ; } while (0)
#endif

// file type
#define TYPE_NONE 0
#define TYPE_FILE 1
// NOTE: TYPE_DIR only for inode and stat
// kernel file_t don't and needn't distingush between file and dir
#define TYPE_DIR  2
#define TYPE_DEV  3

// open mode
#define O_RDONLY  0x000
#define O_WRONLY  0x001
#define O_RDWR    0x002
#define O_CREATE  0x200
#define O_TRUNC   0x400
#define O_DIR     0x800

// seek whence
#define SEEK_SET 0
#define SEEK_CUR 1
#define SEEK_END 2

// file stat
struct stat {
  uint32_t type;
  uint32_t size;
  uint32_t node;
};

#endif
