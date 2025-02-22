/*dynlist *get_nearby_non_walkable_tiles_for_world_position(Vector2 world_position, level *level) {*/
/*  dynlist *nearby_tiles = dynlist_allocate(sizeof(Vector2), 8);*/
/*  int tile_x = (int) floor(world_position.x / level->tile_size);*/
/*  int tile_y = (int) floor(world_position.y / level->tile_size);*/
/**/
/*  // create cardinal positions*/
/*  for (int dy = -1; dy <= 1; dy++) {*/
/*    for (int dx = -1; dx <= 1; dx++) {*/
/*      int x = tile_x + dx;*/
/*      int y = tile_y + dy;*/
/**/
/*      // skip if  out of bounds*/
/*      if (x < 0 || x >= level->width || y < 0 || y >= level->height) {*/
/*        continue;*/
/*      }*/
/**/
/*      int tilemap_index = level->tilemap[y][x];*/
/*      tile_def tile_def = level->tile_defs[tilemap_index];*/
/*      if (!tile_def.is_walkable) {*/
/*        Vector2 tile_position = { (float)x , (float)y };*/
/*        dynlist_append(nearby_tiles, &tile_position);*/
/*      }*/
/*    }*/
/**/
/*  }*/
/*  return nearby_tiles;*/
/*}*/
/**/
/*bool physics_intersect_rect(Rectangle *first, Rectangle *second) {*/
/*  bool intersects = (*/
/*    (first->x < (second->x + second->width) &&  // Left   edge of first is to the right of the right edge of second*/
/*    (first->x + first->width) > second->x) &&   // Right  edge of first is to the left  of the left  edge of second*/
/*    (first->y < (second->y + second->height) && // Top    edge of first is above the bottom edge of second*/
/*    (first->y + first->height) > second->y)     // Bottom edge of first is below the top    edge of second*/
/*  );*/
/**/
/*  return intersects;*/
/*}*/
/**/
/*void physics_update(level *level) {*/
/*  physics_body *body;*/
/**/
/*  for (size_t i = 0; i < state.bodies->length; i++)*/
/*  {*/
/*    body = dynlist_get_at(state.bodies, i);*/
/**/
/*    // Apply acceleration*/
/*    body->acceleration = vector2_mult_by_scalar(body->direction, body->acceleration_factor);*/
/**/
/*    // Reset acceleration if no direction*/
/*    if (vector2_magnitude(body->direction) == 0)*/
/*    {*/
/*      body->acceleration = (Vector2){0.0f, 0.0f};*/
/*    }*/
/**/
/*    {*/
/*      // Store the calculated "proposed" velocity seperately for x and y axis so*/
/*      // we can check for collisions indenpendently, and by that support "sliding*/
/*      // behavior"*/
/**/
/*      float proposed_velocity_x  = body->velocity.x;         // Copy of current velocity (x axis)*/
/*      proposed_velocity_x       += body->acceleration.x;     // Update velocity with acceleration */
/*      proposed_velocity_x       *= (1.0f - body->friction ); // Apply friction to velocity (friction_coefficient is used as 1.0f - MAGIC_NR)*/
/*      Rectangle proposed_aabb_x  = body->aabb;               // Copy of current AABB*/
/*      proposed_aabb_x.x         += proposed_velocity_x;      // Apply proposed velocity to AABB*/
/**/
/*      float proposed_velocity_y  = body->velocity.y;         // Copy of current velocity (y axis)*/
/*      proposed_velocity_y       += body->acceleration.y;     // Update velocity with acceleration*/
/*      proposed_velocity_y       *= (1.0f - body->friction ); // Apply friction to velocity (friction_coefficient is used as 1.0f - MAGIC_NR)*/
/*      Rectangle proposed_aabb_y  = body->aabb;               // Copy of current AABB*/
/*      proposed_aabb_y.y         += proposed_velocity_y;      // Apply proposed velocity to AABB*/
/**/
/*      {*/
/*        dynlist *nearby_non_walkable_tiles = get_nearby_non_walkable_tiles_for_world_position(body->position, level);*/
/**/
/*        bool collision_x = false;*/
/*        bool collision_y = false;*/
/*        for (size_t i = 0; i < nearby_non_walkable_tiles->length; i++) {*/
/*          Vector2 *nearby_non_walkable_tile_pos = dynlist_get_at(nearby_non_walkable_tiles, i);*/
/*          Rectangle nearby_non_walkable_tile = { */
/*            nearby_non_walkable_tile_pos->x * level->tile_size, */
/*            nearby_non_walkable_tile_pos->y * level->tile_size, */
/*            level->tile_size, */
/*            level->tile_size */
/*          };*/
/**/
/*          Rectangle proposed_aabb    = body->aabb;*/
/*          proposed_aabb.x           += proposed_velocity_x;*/
/*          proposed_aabb.y           += proposed_velocity_y;*/
/**/
/*          if (physics_intersect_rect(&proposed_aabb_x, &nearby_non_walkable_tile)) {*/
/*            collision_x = true;*/
/*          }*/
/**/
/*          if (physics_intersect_rect(&proposed_aabb_y, &nearby_non_walkable_tile)) {*/
/*            collision_y = true;*/
/*          }*/
/*        }*/
/**/
/*        dynlist_free(nearby_non_walkable_tiles);*/
/**/
/*        if (collision_x) {*/
/*          // TODO: handle differently for kinematic*/
/*          body->velocity.x = 0.0f;*/
/*        } else {*/
/*          body->velocity.x = proposed_velocity_x;*/
/*        }*/
/**/
/*        if (collision_y) {*/
/*          // TODO: handle differently for kinematic*/
/*          body->velocity.y = 0.0f;*/
/*        } else {*/
/*          body->velocity.y = proposed_velocity_y;*/
/*        }*/
/*      }*/
/*    }*/
/**/
/*    // Lock the velocity to the max speed*/
/*    if (vector2_magnitude(body->velocity) > body->max_speed)*/
/*    {*/
/*      body->velocity = vector2_normalize(body->velocity);*/
/*      body->velocity = vector2_mult_by_scalar(body->velocity, body->max_speed);*/
/*    }*/
/**/
/*    body->position = vector2_add(body->position, body->velocity);*/
/*    body->aabb = physics_get_aabb(body);*/
/*  }*/
/*}*/
