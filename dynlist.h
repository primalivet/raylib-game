#ifndef DYNLIST_H
#define DYNLIST_H

#include <stdlib.h>

#define PRINT_EXIT(C, ...) printf(__VA_ARGS__); exit(C);
#define PRINT_RETURN(R, ...) printf(__VA_ARGS__); return(R);

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
void *dynlist_get_at(dynlist *list, size_t index);
int dynlist_remove_at(dynlist *list, size_t index);

#endif
