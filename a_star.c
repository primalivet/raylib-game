#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include "a_star.h"
#include "physics.h"
#include "dynlist.h"
#include "vector2.h"

static astar_state state;


void astar_allocate(level *level) {
  astar_node **grid = malloc(level->height * sizeof(astar_node *));
  for (int y = 0; y < level->height; y++) {
    grid[y] = malloc(level->width * sizeof(astar_node));
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

#pragma GCC diagnostic ignored "-Wunused-parameter" // TODO: remove as implemented
dynlist *reconstruct_path(astar_node *goal_node) {
  // TODO: path should be a prio_queue
  //dynlist *path = dynlist_allocate(sizeof(astar_node), 0);
  //astar_node *current = goal_node;
  //while (current != NULL) {
  //  dynlist_prepend(path, &current);
  //  current = current->parent;
  //}
  //return path;
  printf("Not yet implemented");
  return NULL;
}

void free_reconstructed_path(dynlist *path) {
  if (!path) return;
  dynlist_free(path);
}

dynlist *astar_search(physics_body *origin, physics_body *target)
{

  float h_cost = manhattan_distance((Vector2){origin->aabb.x, origin->aabb.y}, 
                                    (Vector2){target->aabb.x, target->aabb.y}) / state.tile_size;
  astar_node origin_node = {
    .x = origin->aabb.x / state.tile_size,
    .y = origin->aabb.y / state.tile_size,
    .g_cost = 0,
    .h_cost = h_cost,
    .f_cost =  0 + h_cost,
    .is_walkable = true,
    .parent = NULL,
  };

  astar_node goal_node = {
    .x = origin->aabb.x / state.tile_size,
    .y = origin->aabb.y / state.tile_size,
    .g_cost = INFINITY,
    .h_cost = 0,
    .f_cost = INFINITY + 0,
    .is_walkable = true,
    .parent = NULL,
  };

  dynlist *open_set = dynlist_allocate(sizeof(astar_node), 0);
  dynlist *closed_set = dynlist_allocate(sizeof(astar_node), 0);

  dynlist_append(open_set, &origin_node);

  while(open_set->length != 0) {

    size_t lowest_f_cost_node_index = 0;
    astar_node *lowest_f_cost_node = NULL;
    for (size_t i = 0; i < open_set->length; i++) {
      astar_node *current = (astar_node *)dynlist_get_at(open_set, i);
      if (!lowest_f_cost_node || current->f_cost < lowest_f_cost_node->f_cost) {
        lowest_f_cost_node = current;
      }
    }

    if (lowest_f_cost_node == &goal_node) {
      dynlist* path = reconstruct_path(&goal_node);
      dynlist_free(open_set);
      dynlist_free(closed_set);
      return path;
    }

    dynlist_remove_at(open_set, lowest_f_cost_node_index);
    dynlist_append(closed_set, lowest_f_cost_node);

    // TODO: loop thorugh the grid and get neighbours (N = neighbour, C = current) (max 8)
    // | N | N | N |
    // | N | C | N |
    // | N | N | N |
    dynlist neighbours[8];
    for (int y = 0; y < state.grid_height; y++) {
      for(int x = 0; x < state.grid_width; x++) {
        if (y == lowest_f_cost_node->y || y == lowest_f_cost_node->y - 1 || y == lowest_f_cost_node->y + 1) {
          // it's on the above row, same row, or below row
          if (x == lowest_f_cost_node->x || x == lowest_f_cost_node->x - 1 || x == lowest_f_cost_node->x + 1) {
            // it's on the left column, right column, or right column
            // It's a neightbour
            astar_node *neighbour = &state.grid[y][x];
            dynlist_append(neighbours, neighbour);
          }
        }
      }
    }
  
    for (size_t i = 0; i < neighbours->length; i++) {
      astar_node *neighbour = (astar_node *)dynlist_get_at(neighbours, i);
      bool is_in_closed_set = false;
      bool is_in_open_set = false;

      for (size_t j = 0; j < closed_set->length; j++) {
        astar_node *closed_node = (astar_node *)dynlist_get_at(closed_set, j);
        if (neighbour->x == closed_node->x && neighbour->y == closed_node->y) {
          is_in_closed_set = true;
        }
      }

      for (size_t j = 0; j < open_set->length; j++) {
        astar_node *open_node = (astar_node *)dynlist_get_at(open_set, j);
        if (neighbour->x == open_node->x && neighbour->y == open_node->y) {
          is_in_open_set = true;
        }
      }

      if (!neighbour->is_walkable || is_in_closed_set) {
        continue;
      }

      float tentative_g_cost = lowest_f_cost_node->g_cost + manhattan_distance((Vector2){lowest_f_cost_node->x, lowest_f_cost_node->y}, (Vector2){neighbour->x, neighbour->y}) / state.tile_size;

      if (!is_in_open_set  || tentative_g_cost < neighbour->g_cost) {
        neighbour->parent = lowest_f_cost_node;
        neighbour->g_cost = tentative_g_cost;
        neighbour->h_cost = manhattan_distance((Vector2){neighbour->x, neighbour->y}, (Vector2){target->aabb.x, target->aabb.y});
        neighbour->f_cost = neighbour->g_cost + neighbour->h_cost;

        if (!is_in_open_set) {
          dynlist_append(open_set, neighbour);
        }
      }
    }


  }
  dynlist_free(open_set);
  dynlist_free(closed_set);
  return NULL;
}

void astar_free()
{
  free(state.grid);
  state.grid = NULL;
}
