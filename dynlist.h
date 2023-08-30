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

dynlist *list_create(size_t sizeof_item, size_t initial_capacity);
void list_destroy(dynlist *list);
size_t list_append(dynlist *list, void *item);
void *list_get(dynlist *list, size_t index);
int list_remove(dynlist *list, size_t index);

#endif
