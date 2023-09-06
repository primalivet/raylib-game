#ifndef ASTAR_H
#define ASTAR_H
#include <stdbool.h>
#include "dynlist.h"
#include "level.h"
#include "physics.h"

typedef struct astar_node {
  int x;
  int y;
  float g_cost;              // Cost from the start node
  float h_cost;              // Cost esitmate to the goal (heuristic)
  float f_cost;              // Total cost (g_cost + h_cost)
  bool is_walkable;
  struct astar_node *parent; // Reference to reconstruct the path
} astar_node;

typedef struct {
  int grid_width;
  int grid_height;
  int tile_size;
  astar_node **grid;
} astar_state;


void astar_allocate(level *level);
void astar_free();
dynlist *astar_search(physics_body *origin, physics_body *target);

#endif
