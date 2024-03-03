#include "klib.h"

#define LIST_NUM 256

static list_t list_buf[LIST_NUM];
static list_t free_list;
static int inited = 0;

static void list_add_next(list_t *list, list_t *ptr) {
  ptr->prev = list;
  ptr->next = list->next;
  ptr->prev->next = ptr;
  ptr->next->prev = ptr;
}

static list_t *list_remove_prev(list_t *list) {
  assert(!list_empty(list));
  list_t *ptr = list->prev;
  ptr->prev->next = ptr->next;
  ptr->next->prev = ptr->prev;
  return ptr;
}

static void init_list() {
  list_init(&free_list);
  for (int i = 0; i < LIST_NUM; ++i) {
    list_add_next(&free_list, &list_buf[i]);
  }
}

void list_init(list_t *list) {
  if (!inited) {
    inited = 1;
    init_list();
  }
  list->prev = list->next = list;
}

int list_empty(list_t *list) {
  return list == list->next;
}

list_t *list_enqueue(list_t *list, void *ptr) {
  list_t *l = list_remove_prev(&free_list);
  l->ptr = ptr;
  list_add_next(list, l);
  return l;
}

void *list_dequeue(list_t *list) {
  if (list_empty(list)) {
    return NULL;
  }
  list_t *l = list_remove_prev(list);
  void *ptr = l->ptr;
  list_add_next(&free_list, l);
  return ptr;
}

void list_remove(list_t *list, list_t *entry) {
  entry->next->prev = entry->prev;
  entry->prev->next = entry->next;
  list_add_next(&free_list, entry);
}
