#include <assert.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include "a_star.h"
#include "physics.h"
#include "dynlist.h"
#include "vector2.h"
#include "prio_queue.h"
#include "assert.h"

static astar_state state;

void astar_allocate(level *level) {
  // TODO: this can not be of type astar_node, since we have to generate them in the algorithm
  // This should rather be just a grid of number, or maybe a tile_defs
  astar_node **grid = malloc(level->height * sizeof(astar_node *));
  if (!grid) {
    printf("Failed to allocate grid\n");
    exit(1);
  }
  for (int y = 0; y < level->height; y++) {
    grid[y] = malloc(level->width * sizeof(astar_node));
    if (!grid[y]) {
      printf("Failed to allocate grid row %d\n", y);
    }
    for (int x = 0; x < level->width; x++) {
      grid[y][x].x = x;
      grid[y][x].y = y;
      grid[y][x].is_walkable = level->tile_defs[level->tilemap[y][x]].is_walkable;
    }
  }

  state.grid_width = level->width;
  state.grid_height = level->height;
  state.tile_size = level->tile_size;
  state.grid = grid;
}

dynlist *reconstruct_path(astar_node *goal_node) {
  dynlist *path = dynlist_allocate(sizeof(astar_node), 0);
  astar_node *current = goal_node;

  while (current != NULL) {
    dynlist_prepend(path, current);
    current = current->parent;
  }

  return path;
}

void free_reconstructed_path(dynlist *path) {
  if (!path) return;
  dynlist_free(path);
}

bool is_valid_grid_position(Vector2 position) {
  return (position.x >= 0 && 
  position.x <= state.grid_width &&
  position.y >= 0 &&
  position.x <= state.grid_height);
}

dynlist *astar_search(Rectangle *origin, Rectangle *goal)
{
  prio_queue *open_set   = queue_allocate(sizeof(astar_node), 0.5f);
  prio_queue *closed_set = queue_allocate(sizeof(astar_node), 0.5f);

  float h_cost = manhattan_distance((Vector2){origin->x, origin->y}, 
                                    (Vector2){goal->x, goal->y}) / state.tile_size;

  astar_node *origin_node  = malloc(sizeof(astar_node));
  if (!origin_node) { printf("Failed to allocate node origin\n"); exit(1); }
  origin_node->x           = origin->x / state.tile_size;
  origin_node->y           = origin->y / state.tile_size;
  origin_node->g_cost      = 0;
  origin_node->h_cost      = h_cost;
  origin_node->f_cost      =  0;
  origin_node->is_walkable = true;
  origin_node->parent      = NULL;

  astar_node *goal_node  = malloc(sizeof(astar_node));
  if (!goal_node) { printf("Failed to allocate node goal\n"); exit(1); }
  goal_node->x           = goal->x / state.tile_size;
  goal_node->y           = goal->y / state.tile_size;
  goal_node->g_cost      = INFINITY;
  goal_node->h_cost      = 0;
  goal_node->f_cost      = INFINITY + 0;
  goal_node->is_walkable = true;
  goal_node->parent      = NULL;

  // Create the staring prio item and att to the open set

  prio_item *origin_item = malloc(sizeof(prio_item));
  if (!origin_item) { printf("Failed to allocate origin item\n"); exit(1); }
  origin_item->priority  = -origin_node->f_cost;
  origin_item->data      = origin_node;
  queue_enqueue(open_set, origin_item);

  while(open_set->length != 0) {
    // Get the highest prio item data (lowest f_cost)
    prio_item *q_item = queue_dequeue(open_set);
    astar_node *q = q_item->data;

    // Generate cardinal positions around q
    Vector2 cardinal_positions[8]; 
    cardinal_positions[0] = (Vector2){ q->x - 1, q->y - 1}; // North West
    cardinal_positions[1] = (Vector2){ q->x,     q->y - 1}; // North
    cardinal_positions[2] = (Vector2){ q->x + 1, q->y - 1}; // North East
    cardinal_positions[3] = (Vector2){ q->x - 1, q->y    }; // West
    cardinal_positions[4] = (Vector2){ q->x + 1, q->y    }; // East
    cardinal_positions[5] = (Vector2){ q->x - 1, q->y + 1}; // South West
    cardinal_positions[6] = (Vector2){ q->x,     q->y + 1}; // South
    cardinal_positions[7] = (Vector2){ q->x + 1, q->y + 1}; // South East

    astar_node **neighbours = malloc(8 * sizeof(astar_node *));
    if (!neighbours) { printf("Failed to allocate neighbours\n"); exit(1); }

    // Generate neighbours from cardinal positions
    int neighbours_count = 0;
    for (int i = 0; i < 8; i++) {
      if (is_valid_grid_position(cardinal_positions[i])) {
        astar_node *neighbour = malloc(sizeof(astar_node));
        if (!neighbour) { printf("Failed to allocate neighbour\n"); exit(1); }
        Vector2 q_pos         = (Vector2){ q->x, q->y };
        Vector2 neighbour_pos = (Vector2){ cardinal_positions[i].x, cardinal_positions[i].y };
        float g_cost          = q->g_cost + manhattan_distance(q_pos, neighbour_pos);
        float h_cost          = manhattan_distance(neighbour_pos, (Vector2){ goal->x, goal->y });
        float f_cost          = g_cost + neighbour->h_cost;
        neighbour->x          = neighbour_pos.x;
        neighbour->y          = neighbour_pos.y;
        neighbour->g_cost     = g_cost;
        neighbour->h_cost     = h_cost;
        neighbour->f_cost     = f_cost;
        neighbour->parent     = q;

        // Add neighbour to neighbours arrayt. Increase count only if we append to the array
        neighbours[neighbours_count++] = neighbour; 

        // Base case / We found the goal, stop searching
        if (neighbour->x == goal_node->x && neighbour->y == goal_node->y) {
          return reconstruct_path(neighbour);
        }


        bool skip_neighbour             = false;
        bool neighbour_is_in_open_set   = false;
        bool neighbour_is_in_closed_set = false;

        // Search for neighbour in open_set
        for (size_t i = 0; i < open_set->length; i++) {
          astar_node *open_set_node = open_set->items[i].data;
          neighbour_is_in_open_set = open_set_node->x == neighbour->x && open_set_node->y == neighbour->y;
          bool existing_has_lower_f_cost = open_set_node->f_cost <= neighbour->f_cost;
          if (neighbour_is_in_open_set && existing_has_lower_f_cost) {
            skip_neighbour = true;
          }
        }

        // Search for neighbour in closed_set
        for (size_t i = 0; i < closed_set->length; i++) {
          astar_node *closed_set_node = closed_set->items[i].data;
          neighbour_is_in_closed_set = closed_set_node->x == neighbour->x && closed_set_node->y == neighbour->y;
          bool existing_has_lower_f_cost = closed_set_node->f_cost <= neighbour->f_cost;
          if (neighbour_is_in_closed_set && existing_has_lower_f_cost) {
            skip_neighbour = true;
          }
        }

        if (!skip_neighbour) {
          prio_item *p_item = malloc(sizeof(prio_item));
          if (!p_item) { printf("Failed to allocate p_item\n"); exit(1); }
          p_item->priority = -neighbour->f_cost;
          p_item->data     = neighbour;
          queue_enqueue(open_set, p_item);
        } else {
          // Free skipped neighbour
          free(neighbour);
        }
      }
    }

    // Free neighbours array
    if (neighbours) { free(neighbours); }
  }

  // Free nodes in the open set
  for (size_t i = 0; i < open_set->length; i++) 
    if (open_set->items[i].data)  { free(open_set->items[i].data); }

  // Free nodes in the closed set
  for (size_t i = 0; i < closed_set->length; i++) 
    if (closed_set->items[i].data)  { free(closed_set->items[i].data); }

  // Free the open and closed set
  queue_free(open_set);
  queue_free(closed_set);

  // Free node origin and node goal
  if (origin_node != NULL) { free(origin_node); }
  if (goal_node   != NULL) { free(goal_node); }

  // Did not find the goal
  return NULL;
}

void astar_free()
{
  free(state.grid);
  state.grid = NULL;
}
