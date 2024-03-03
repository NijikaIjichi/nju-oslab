#ifndef __KLIB_H__
#define __KLIB_H__
#define DEBUG // comment me if need

#include "lib.h"
#include "x86.h"

void putstr(const char *str);
void putstrn(const char *str, size_t count);
int printf(const char *format, ...);

#ifdef DEBUG
#  define Log(fmt, ...) printf("[%s:%d] " fmt, __FILE__, __LINE__, ##__VA_ARGS__)
#else
#  define Log(...) ((void)0)
#endif

int abort(const char *file, int line, const char *info) __attribute__((noreturn));

typedef struct list {
  void *ptr;
  struct list *prev, *next;
} list_t;

void list_init(list_t *list);
int list_empty(list_t *list);
list_t *list_enqueue(list_t *list, void *ptr);
void *list_dequeue(list_t *list);
void list_remove(list_t *list, list_t *entry);

#endif
