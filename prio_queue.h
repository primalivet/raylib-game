#ifndef PRIO_QUEUE_H
#define PRIO_QUEUE_H
#include <stdlib.h>

// The priority item - can hold any data with a "generic void pointer" to
// something allocated by the caller.
typedef struct prio_item {
  // The priority of a queue item
  float priority;
  // The data "generic void pointer"
  void *data;
} prio_item;

// The priority queue - It works by keeping a list of items ordered by priority
// always dequeuing the max priority item
typedef struct {
  // The current length of the queue
  size_t length;
  // The current capacity of the queue
  size_t capacity;
  // Max "load factor" of the capacity
  float load_factor;
  // The priority items
  prio_item *items;
} prio_queue;


// Create a new priority queue
prio_queue *queue_allocate(size_t initial_capacity, float load_factor);

// Destroy and free the memory of the priority queue
void queue_free(prio_queue *queue);

// Add item to the priority queue
void queue_enqueue(prio_queue *queue, prio_item *item);

// Remove item from the priority queue
prio_item *queue_dequeue(prio_queue *queue);

// Free the data of the item "generic void pointer"
void queue_free_item_data(prio_item *item);

#endif
