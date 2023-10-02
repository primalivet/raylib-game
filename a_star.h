#ifndef ASTAR_H
#define ASTAR_H
#include <stdbool.h>
#include "dynlist.h"
#include "level.h"
#include "physics.h"
#include "prio_queue.h"

typedef enum {
  ADD_TO_OPEN,
  ADD_TO_CLOSED,
  REPLACE_IN_OPEN,
  SKIP
} node_action;

typedef struct astar_node {
  float x;
  float y;
  float g_cost;              // Cost from the start node
  float h_cost;              // Cost esitmate to the goal (heuristic)
  float f_cost;              // Total cost (g_cost + h_cost)
  bool is_walkable;
  struct astar_node *parent; // Reference to reconstruct the path
} astar_node;

typedef struct {
  int grid_width;
  int grid_height;
  astar_node **grid;
  prio_queue *opened;
  prio_queue *closed;
} astar_state;


void astar_allocate(int width, int height);
int astar_compare_nodes(const void *a, const void *b);
void free_reconstructed_path(dynlist *path);
void astar_free();
dynlist *astar_search(Vector2 *origin, Vector2 *target);

#endif