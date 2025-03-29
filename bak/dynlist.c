#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "utils.h"
#include "dynlist.h"

dynlist *dynlist_allocate(size_t sizeof_item, size_t initial_capacity) 
{
  dynlist *list = (dynlist *)malloc(sizeof(dynlist));
  if (!list) 
  {
    printf("Failed to allocate memory for dynlist");
    exit(1);
  }

  list->items = malloc(sizeof_item * initial_capacity);
  if (!list->items) 
  { 
    free(list); 
    printf("Failed to allocate memory for dynlist->items\n");
    exit(1);
  }
  list->capacity    = initial_capacity;
  list->length      = 0;
  list->sizeof_item = sizeof_item;
  return list;
}

void dynlist_free(dynlist *list) 
{
  if (list) {
    if (list->items){
      free(list->items);
    }
    free(list);
  }
}

size_t dynlist_append(dynlist *list, void *item) 
{
  if (list->length == list->capacity)
  {
    list->capacity = list->capacity > 0 ?  list->capacity * 2 : 1; // double the capacity, account for 0 * 2 = 0
    void *items = (void *)realloc(list->items, list->capacity * list->sizeof_item);
    if (!items) 
    { 
      printf("Failed to reallocate memory for expanding capacity of dynlist\n");
      exit(1);
    }
    list->items    = items;
  }
  size_t index = list->length++;
  memcpy(list->items + index * list->sizeof_item, item, list->sizeof_item);
  return index;
}

size_t dynlist_prepend(dynlist *list, void *item) 
{
  if (list->length == list->capacity)
  {
    list->capacity = list->capacity > 0 ?  list->capacity * 2 : 1; // double the capacity, account for 0 * 2 = 0
    void *items = (void *)realloc(list->items, list->capacity * list->sizeof_item);
    if (!items) 
    { 
      printf("Failed to reallocate memory for expanding capacity of dynlist\n");
      exit(1);
    }
    list->items    = items;
  }

  memmove(
    list->items + list->sizeof_item, // Where to move to (the second position in the list)
    list->items,                     // Where to start moving from (the start of the list)
    list->length * list->sizeof_item // Number of bytes to move (the whole list)
  );

  memcpy(
    list->items,      // Dest
    item,             // Src
    list->sizeof_item // Size
  );

  list->length++;

  return 0; // always return the first position, since we're prepending
}

void *dynlist_get_at(dynlist *list, size_t index)
{
  if (index >= list->length) 
  {
    printf( "Failed to retrive item in dynlist as index is out of bounds\n");
    return NULL;
  }
  return list->items + index * list->sizeof_item;
}

int dynlist_remove_at(dynlist *list, size_t index)
{
  if (list->length == 0) 
  {
    printf("Falied to remove item in dynlist as it is empty\n");
    return -1;
  }
  if (index >= list->length) 
  {
    printf("Falied to remove item in dynlist as index is out of bounds\n");
    return -1;
  }
  --list->length;                                                              // decrease length
  char *removed = (char*)list->items + index * list->sizeof_item;              // take the item to be removed
  char *last    = (char*)list->items + (list->length - 1) * list->sizeof_item; // take the last item
  memcpy(removed, last, list->sizeof_item);                                    // place the last item at the index of the item to be removed
  return 0;
}
