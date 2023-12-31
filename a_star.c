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

/**
 * Compare the cost of two astar nodes, both f_cost and h_cost are considered
 *
 * In general a sorting algorithm should be returning -1 if a is less than b,
 * but we want the lower one, not the highest, so prio is set as negative
 * numbers in astar
 *
 * returns -1 if a is less than b
 * returns 0 if a is equal to b
 * returns 1 if a is greater than b
 */
int astar_compare_nodes(const void *a, const void *b) {
  prio_item *item_a = (prio_item *)a;
  prio_item *item_b = (prio_item *)b;
  astar_node *node_a = item_a->data;
  astar_node *node_b = item_b->data;


  // Compare by Full cost
  if (node_a->f_cost < node_b->f_cost) return -1;
  if (node_a->f_cost > node_b->f_cost) return 1;

  // Compare by Heuristic cost
  if (node_a->h_cost < node_b->h_cost) return -1;
  if (node_a->h_cost > node_b->h_cost) return 1;

  return 0;
}

dynlist *reconstruct_path(astar_node *goal_node) {
  dynlist *path = dynlist_allocate(sizeof(Vector2), 0);
  astar_node *current = goal_node;

  while (current != NULL) {
    dynlist_prepend(path, &(Vector2){ current->x, current->y });
    current = current->parent;
  }

  return path;
}

void free_reconstructed_path(dynlist *path) {
  if (!path) return;
  dynlist_free(path);
}

bool is_valid_position(level *level, Vector2 position) {
  int x = (int)position.x;
  int y = (int)position.y;
  bool is_within_bounds = position.x >= 0 && 
                          position.x <= level->width &&
                          position.y >= 0 &&
                          position.y <= level->height;
  if (is_within_bounds) {
    bool is_walkable = level->collision_mask[y][x];
    return is_walkable;
  } else {
    return false;
  }
}

int get_queued_node_by_pos(prio_queue *queue, Vector2 pos) {
  int index = -1;
  for (size_t i = 0; i < queue->length; i++) {
    astar_node *current = queue->items[i].data;
    if (current->x == pos.x && current->y == pos.y) {
      index = i;
    }
  }
  return index;
}

dynlist *astar_search(level *level, Vector2 *origin, Vector2 *goal)
{
  prio_queue *open_set   = queue_allocate(10, 0.5f, astar_compare_nodes);
  prio_queue *closed_set = queue_allocate(10, 0.5f, astar_compare_nodes);

  float h_cost = vector2_euclidean_distance((Vector2){origin->x, origin->y}, 
                                    (Vector2){goal->x, goal->y});

  astar_node *origin_node  = malloc(sizeof(astar_node));
  if (!origin_node) { printf("Failed to allocate node origin\n"); exit(1); }
  origin_node->x           = origin->x;
  origin_node->y           = origin->y;
  origin_node->g_cost      = 0;
  origin_node->h_cost      = h_cost;
  origin_node->f_cost      =  0;
  origin_node->parent      = NULL;

  astar_node *goal_node  = malloc(sizeof(astar_node));
  if (!goal_node) { printf("Failed to allocate node goal\n"); exit(1); }
  goal_node->x           = goal->x;
  goal_node->y           = goal->y;
  goal_node->g_cost      = INFINITY;
  goal_node->h_cost      = 0;
  goal_node->f_cost      = INFINITY + 0;
  goal_node->parent      = NULL;

  // Create the staring prio item and att to the open set

  prio_item *origin_item = malloc(sizeof(prio_item));
  if (!origin_item) { printf("Failed to allocate origin item\n"); exit(1); }
  origin_item->priority  = -origin_node->f_cost;
  origin_item->data      = origin_node;
  queue_enqueue(open_set, origin_item);

  int iteration = 1;
  int max_iterations = 100;
  while(open_set->length != 0 && iteration < max_iterations) {
    // Get the highest prio item data (lowest f_cost)

    prio_item *q_item = queue_dequeue(open_set);
    astar_node *q = q_item->data;


    if (q->x == goal_node->x && q->y == goal_node->y) {
      return reconstruct_path(q);
    }

    queue_enqueue(closed_set, q_item);

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

    bool valid_cardinal_positions[8];
    valid_cardinal_positions[1] = is_valid_position(level, cardinal_positions[1]); // North
    valid_cardinal_positions[3] = is_valid_position(level, cardinal_positions[3]); // West
    valid_cardinal_positions[4] = is_valid_position(level, cardinal_positions[4]); // East
    valid_cardinal_positions[6] = is_valid_position(level, cardinal_positions[6]); // South

    valid_cardinal_positions[0] = is_valid_position(level, cardinal_positions[0]) && valid_cardinal_positions[1] && valid_cardinal_positions[3]; // North West (and North and West)
    valid_cardinal_positions[2] = is_valid_position(level, cardinal_positions[2]) && valid_cardinal_positions[1] && valid_cardinal_positions[4]; // North East (and North and East)
    valid_cardinal_positions[5] = is_valid_position(level, cardinal_positions[5]) && valid_cardinal_positions[6] && valid_cardinal_positions[3]; // South West (and South and West)
    valid_cardinal_positions[7] = is_valid_position(level, cardinal_positions[7]) && valid_cardinal_positions[6] && valid_cardinal_positions[4]; // South East (and South and East)

    astar_node **neighbours = malloc(8 * sizeof(astar_node *));
    if (!neighbours) { printf("Failed to allocate neighbours\n"); exit(1); }

    // Generate neighbours from cardinal positions
    int neighbours_count = 0;
    for (int i = 0; i < 8; i++) {
      int existing_closed_node_index = get_queued_node_by_pos(closed_set, cardinal_positions[i]);
      if (existing_closed_node_index != -1) {
        continue;
      }
      if (valid_cardinal_positions[i]) {
        astar_node *neighbour = malloc(sizeof(astar_node));
        if (!neighbour) { printf("Failed to allocate neighbour\n"); exit(1); }
        Vector2 goal_pos      = (Vector2){ goal->x, goal->y };
        Vector2 neighbour_pos = (Vector2){ cardinal_positions[i].x, cardinal_positions[i].y };
        bool is_diagonal      = false;
        if (neighbour_pos.y < q->y && neighbour_pos.x < q->x) { is_diagonal = true; } // North West
        if (neighbour_pos.y < q->y && neighbour_pos.x > q->x) { is_diagonal = true; } // North East
        if (neighbour_pos.y > q->y && neighbour_pos.x < q->x) { is_diagonal = true; } // South West
        if (neighbour_pos.y > q->y && neighbour_pos.x > q->x) { is_diagonal = true; } // South East

        float g_cost          = 1;
        if (is_diagonal) {
          g_cost = 1.414f;
        }
        float h_cost          = vector2_euclidean_distance(neighbour_pos, goal_pos);
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

        int existing_open_node_index = get_queued_node_by_pos(open_set, neighbour_pos);
        if (existing_open_node_index != -1) {
          astar_node *existing_open_node = open_set->items[existing_open_node_index].data;
          if (neighbour->g_cost < existing_open_node->g_cost) {
            handle_node = REPLACE_IN_OPEN;
          } else {
            handle_node = SKIP;
          }
        } 

        prio_item *p_item = malloc(sizeof(prio_item));
        if (!p_item) { printf("Failed to allocate p_item\n"); exit(1); }
        p_item->priority = -neighbour->f_cost;
        p_item->data     = neighbour;

        switch(handle_node) {
          case ADD_TO_OPEN: { 
            queue_enqueue(open_set, p_item);
          } break; 
          case ADD_TO_CLOSED: { 
            queue_enqueue(closed_set, p_item); 
          } break; 
          case REPLACE_IN_OPEN: { 
            queue_replace_at(open_set, existing_open_node_index, p_item); 
          } break;
          case SKIP: { 
            if (p_item != NULL) {
              free(p_item); 
              p_item = NULL;
            }
            if (neighbour != NULL) {
              free(neighbour); 
              neighbour = NULL;
            }
          } break; 
        }
      } 
    }
    iteration++;

    // Free neighbours array
    if (neighbours != NULL) { 
      free(neighbours);
      neighbours = NULL; 
    }
  }

  // Free nodes in the open set
  for (size_t i = 0; i < open_set->length; i++) 
    if (open_set->items[i].data != NULL)  { 
      free(open_set->items[i].data); 
      open_set->items[i].data = NULL;
    }

  // Free nodes in the closed set
  for (size_t i = 0; i < closed_set->length; i++) 
    if (closed_set->items[i].data != NULL)  { 
      free(closed_set->items[i].data); 
      closed_set->items[i].data = NULL;
    }

  // Free the open and closed set
  queue_free(open_set);
  open_set = NULL;
  queue_free(closed_set);
  closed_set = NULL;

  // Free node origin and node goal
  if (goal_node   != NULL) { 
    free(goal_node); 
    goal_node   = NULL;
  }

  // Did not find the goal
  return NULL;
}
