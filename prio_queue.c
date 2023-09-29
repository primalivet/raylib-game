#include <stdlib.h>
#include <stdio.h>
#include "prio_queue.h"

prio_queue *queue_allocate(size_t initial_capacity, float load_factor, int (*compare)(const void* a, const void* b)) {
  prio_queue *queue = (prio_queue *)malloc(sizeof(prio_queue));
  if (queue == NULL) {
    printf("Failed to allocate memory for queue\n");
    exit(1);
  }

  if (initial_capacity == 0) {
    initial_capacity = 1; // No use to start with a capacity of 0
  }

  if (load_factor <= 0.0f) {
    load_factor = 0.5f; // Cant use load_factor of 0
  }

  queue->items = (prio_item *)malloc(initial_capacity * sizeof(prio_item));
  if (queue->items == NULL) {
    free(queue);
    printf("Failed to allocate memory for queue->items\n");
    exit(1);
  }

  queue->capacity = initial_capacity;
  queue->length = 0;
  queue->load_factor = load_factor;
  queue->compare = compare;
  return queue;
}

void queue_free(prio_queue *queue) {
  if (queue) {
    if (queue->items){
      free(queue->items);
    }
    free(queue);
  }
}

void queue_enqueue(prio_queue *queue, prio_item *item) {
  float current_load_factor = queue->length / (float)queue->capacity;
  if (current_load_factor >= queue->load_factor) {
    size_t increased_capacity = queue->capacity * 2;
    void *items               = (prio_item *)realloc(queue->items, increased_capacity * sizeof(prio_item));
    if (items == NULL) {
      printf("Failed to reallocate memory for queue->items\n");
      exit(1);
    }
    queue->items = items;
    queue->capacity = increased_capacity;
  }

  queue->items[queue->length] = *item;
  queue->length++;
}

void queue_replace_at(prio_queue *queue, size_t index, prio_item *item) {
  if (index >= queue->length) {
    printf("Index out of bounds for queue_replace_at\n");
    exit(1);
  }
  queue->items[index] = *item;
}

prio_item *queue_dequeue(prio_queue *queue) {
  if (queue->length == 0) {
    printf("Cant dequeue from an empty queue\n");
    return NULL;
  }

  prio_item *highest = NULL;
  int index = 0;
  for (size_t i = 0; i < queue->length; i++) {
    if (highest == NULL) {
      highest = &queue->items[i];
      index = i;
      continue;
    } 

    int result = queue->compare(highest, &queue->items[i]);

    if (result < 0) {
      highest = &queue->items[i];
    } 
  }

  queue->length--;
  for(size_t i = index; i < queue->length; i++) {
    queue->items[i] = queue->items[i + 1];
  }

  return highest;
}

void queue_free_item_data(prio_item *item) {
  if (item && item->data) {
    free(item->data);
  }
}
