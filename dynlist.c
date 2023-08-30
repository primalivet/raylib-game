#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "utils.h"
#include "dynlist.h"

dynlist *list_create(size_t sizeof_item, size_t initial_capacity) 
{
  dynlist *list = (dynlist *)malloc(sizeof(dynlist));
  if (!list) 
  {
    PRINT_EXIT(1, "Failed to allocate memory for dynlist");
  }

  list->items = malloc(sizeof_item * initial_capacity);
  if (!list->items) 
  { 
    free(list); 
    PRINT_EXIT(1, "Failed to allocate memory for dynlist->items\n");
  }
  list->capacity    = initial_capacity;
  list->length      = 0;
  list->sizeof_item = sizeof_item;
  return list;
}

void list_destroy(dynlist *list) 
{
  if (list) {
    if (list->items){
      free(list->items);
    }
    free(list);
  }
}

size_t list_append(dynlist *list, void *item) 
{
  if (list->length == list->capacity)
  {
    list->capacity = list->capacity > 0 ?  list->capacity * 2 : 1; // double the capacity, account for 0 * 2 = 0
    void *items = (void *)realloc(list->items, list->capacity * list->sizeof_item);
    if (!items) 
    { 
      PRINT_EXIT(1, "Failed to reallocate memory for expanding capacity of dynlist\n");
    }
    list->items    = items;
  }
  size_t index = list->length++;
  memcpy(list->items + index * list->sizeof_item, item, list->sizeof_item);
  return index;
}

void *list_get(dynlist *list, size_t index)
{
  if (index >= list->length) 
  {
    PRINT_RETURN(NULL, "Failed to retrive item in dynlist as index is out of bounds\n");
  }
  return list->items + index * list->sizeof_item;
}

int list_remove(dynlist *list, size_t index)
{
  if (list->length == 0) 
  {
    PRINT_RETURN(-1, "Falied to remove item in dynlist as it is empty\n");
  }
  if (index >= list->length) 
  {
    PRINT_RETURN(-1, "Falied to remove item in dynlist as index is out of bounds\n");
  }
  --list->length;                                                   // decrease length
  char *removed = (char*)list->items + index * list->sizeof_item;        // take the item to be removed
  char *last    = (char*)list->items + (list->length - 1) * list->sizeof_item; // take the last item
  memcpy(removed, last, list->sizeof_item);                         // place the last item at the index of the item to be removed
  return 0;
}
