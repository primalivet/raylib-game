#ifndef DYNLIST_H
#define DYNLIST_H

#include <stdlib.h>

typedef struct 
{
  size_t length;
  size_t capacity;
  size_t sizeof_item;
  void *items;
} dynlist;

dynlist *dynlist_allocate(size_t sizeof_item, size_t initial_capacity);
void dynlist_free(dynlist *list);
size_t dynlist_append(dynlist *list, void *item);
size_t dynlist_prepend(dynlist *list, void *item);
void *dynlist_get_at(dynlist *list, size_t index);
int dynlist_remove_at(dynlist *list, size_t index);

#endif
