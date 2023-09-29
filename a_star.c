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

/**
 * Compare the cost of two astar nodes, both f_cost and h_cost are considered
 *
 * returns -1 if a is less than b
 * returns 0 if a is equal to b
 * returns 1 if a is greater than b
 */
int astar_compare_nodes(const void *a, const void *b) {
  astar_node *node_a = (astar_node *)a;
  astar_node *node_b = (astar_node *)b;

  // Compare by Full cost
  if (node_a->f_cost < node_b->f_cost) return -1;
  if (node_a->f_cost > node_b->f_cost) return 1;

  // Compare by Heuristic cost
  if (node_a->h_cost < node_b->h_cost) return -1;
  if (node_a->h_cost > node_b->h_cost) return 1;

  return 0;
}

void astar_allocate(int width, int height) {
  // TODO: this can not be of type astar_node, since we have to generate them in the algorithm
  // This should rather be just a grid of number, or maybe a tile_defs
  astar_node **grid = malloc(height * sizeof(astar_node *));
  if (!grid) {
    printf("Failed to allocate grid\n");
    exit(1);
  }
  for (int y = 0; y < height; y++) {
    grid[y] = malloc(width * sizeof(astar_node));
    if (!grid[y]) {
      printf("Failed to allocate grid row %d\n", y);
    }
    for (int x = 0; x < width; x++) {
      grid[y][x].x = x;
      grid[y][x].y = y;
    }
  }

  state.grid_width = width;
  state.grid_height = height;
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
  return (position.x > 0 && 
  position.x < state.grid_width &&
  position.y > 0 &&
  position.x < state.grid_height);
}

dynlist *astar_search(Vector2 *origin, Vector2 *goal)
{
  prio_queue *open_set   = queue_allocate(sizeof(astar_node), 0.5f, astar_compare_nodes);
  prio_queue *closed_set = queue_allocate(sizeof(astar_node), 0.5f, astar_compare_nodes);

  float h_cost = manhattan_distance((Vector2){origin->x, origin->y}, 
                                    (Vector2){goal->x, goal->y});

  astar_node *origin_node  = malloc(sizeof(astar_node));
  if (!origin_node) { printf("Failed to allocate node origin\n"); exit(1); }
  origin_node->x           = origin->x;
  origin_node->y           = origin->y;
  origin_node->g_cost      = 0;
  origin_node->h_cost      = h_cost;
  origin_node->f_cost      =  0;
  origin_node->is_walkable = true;
  origin_node->parent      = NULL;

  astar_node *goal_node  = malloc(sizeof(astar_node));
  if (!goal_node) { printf("Failed to allocate node goal\n"); exit(1); }
  goal_node->x           = goal->x;
  goal_node->y           = goal->y;
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

    printf("Current:  (x: %d, y: %d) hcost: %f, gcost: %f, fcost: %f\n", q->x, q->y, q->h_cost, q->g_cost, q->f_cost);
    // Generate neighbours from cardinal positions
    int neighbours_count = 0;
    for (int i = 0; i < 8; i++) {
      if (is_valid_grid_position(cardinal_positions[i])) {
        astar_node *neighbour = malloc(sizeof(astar_node));
        if (!neighbour) { printf("Failed to allocate neighbour\n"); exit(1); }
        Vector2 q_pos         = (Vector2){ q->x, q->y };
        Vector2 goal_pos      = (Vector2){ goal->x, goal->y };
        Vector2 neighbour_pos = (Vector2){ cardinal_positions[i].x, cardinal_positions[i].y };
        float g_cost          = q->g_cost + manhattan_distance(neighbour_pos, q_pos);
        float h_cost          = manhattan_distance(neighbour_pos, goal_pos);
        float f_cost          = g_cost + h_cost;
        neighbour->x          = neighbour_pos.x;
        neighbour->y          = neighbour_pos.y;
        neighbour->g_cost     = g_cost; // Goal cost
        neighbour->h_cost     = h_cost; // Heuristic cost
        neighbour->f_cost     = f_cost; // Full cost
        neighbour->parent     = q;

        // Add neighbour to neighbours arrayt. Increase count only if we append to the array
        neighbours[neighbours_count++] = neighbour; 

        // Base case / We found the goal, stop searching
        if (neighbour->x == goal_node->x && neighbour->y == goal_node->y) {
          return reconstruct_path(neighbour);
        }

        node_action handle_node = ADD_TO_OPEN;
        int existing_index = -1;

        // Search for neighbour in open_set
        for (size_t i = 0; i < open_set->length; i++) {
          astar_node *open_set_node     = open_set->items[i].data;
          bool already_exists           = open_set_node->x       == neighbour->x && open_set_node->y == neighbour->y;
          bool existing_has_higher_prio = open_set_node->f_cost > neighbour->f_cost;
          if (already_exists) {
            existing_index = i;
            if (!existing_has_higher_prio) handle_node = REPLACE_IN_OPEN; 
            if (existing_has_higher_prio) handle_node = SKIP;
          }
        }
        // Search for neighbour in closed_set
        for (size_t i = 0; i < closed_set->length; i++) {
          astar_node *closed_set_node     = closed_set->items[i].data;
          bool already_exists             = closed_set_node->x == neighbour->x && closed_set_node->y == neighbour->y;
          bool existing_has_higher_prio   = closed_set_node->f_cost > neighbour->f_cost;
          if (already_exists) {
            existing_index = i;
            if (!existing_has_higher_prio) handle_node = REPLACE_IN_CLOSED;
            if (existing_has_higher_prio) handle_node = SKIP; // skip
          }
        }

        prio_item *p_item = malloc(sizeof(prio_item));
        if (!p_item) { printf("Failed to allocate p_item\n"); exit(1); }
        p_item->priority = -neighbour->f_cost;
        p_item->data     = neighbour;

        printf("Neighbour (x: %d, y: %d) hcost: %f, gcost: %f, fcost: %f\n", neighbour->x, neighbour->y, neighbour->h_cost, neighbour->g_cost, neighbour->f_cost);

        switch(handle_node) {
          case ADD_TO_OPEN       : { queue_enqueue(open_set, p_item);} break; 
          case REPLACE_IN_OPEN   : { queue_replace_at(open_set, existing_index, p_item); } break; 
          case REPLACE_IN_CLOSED : { queue_replace_at(closed_set, existing_index, p_item); } break; 
          case SKIP              : { free(p_item); free(neighbour); } break; 
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
