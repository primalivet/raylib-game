#include <stdlib.h>
#include <math.h>
#include "raylib.h"
#include "entity.h"
#include "level.h"
#include "vector2.h"

void physics_update(entities_t *entities, level_t *level) {
    // Update physics for all entities
    for (int i = 0; i < entities->entities_count; i++) {
        entity_t *entity = entities->entities[i];
        entity_physics_comp_t *physics_body = &entity->physics;
        
        // Apply direction from input
        if (entity->type == ENTITY_TYPE_PLAYER) {
            entity_input_comp_t *input = &entity->player.input;
            
            if (input != NULL) {
                physics_body->direction.x = 0.0f;
                physics_body->direction.y = 0.0f;
                
                if (input->up)    physics_body->direction.y -= 1.0f;
                if (input->down)  physics_body->direction.y += 1.0f;
                if (input->left)  physics_body->direction.x -= 1.0f;
                if (input->right) physics_body->direction.x += 1.0f;
            }
        } else {
            switch(entity->npc.behaviour) {
                case ENTITY_BEHAVIOUR_PATROL: 
                    // TODO: update to more complex behaviour
                    if (physics_body->direction.x == 0.0f && physics_body->direction.y == 0.0f) {
                        int random_x = rand() % 2;
                        int random_y = rand() % 2;
                        physics_body->direction.x = (float)random_x;
                        physics_body->direction.y = (float)random_y;
                    }
                    break;
                default:
                case ENTITY_BEHAVIOUR_STATIONARY:
                    physics_body->direction.x = 0.0f;
                    physics_body->direction.y = 0.0f;
                    break;
            }
        }
        
        // Normalize direction
        float length = sqrt(physics_body->direction.x * physics_body->direction.x + physics_body->direction.y * physics_body->direction.y);
        if (length != 0.0f) {
            physics_body->direction.x /= length;
            physics_body->direction.y /= length;
        }
        
        // Apply friction (down to but not under 0.0f, prevent sliding)
        if      (physics_body->velocity.y < 0.0f) physics_body->velocity.y = fmin(physics_body->velocity.y + physics_body->friction, 0.0f);
        else if (physics_body->velocity.y > 0.0f) physics_body->velocity.y = fmax(physics_body->velocity.y - physics_body->friction, 0.0f);
        if      (physics_body->velocity.x < 0.0f) physics_body->velocity.x = fmin(physics_body->velocity.x + physics_body->friction, 0.0f);
        else if (physics_body->velocity.x > 0.0f) physics_body->velocity.x = fmax(physics_body->velocity.x - physics_body->friction, 0.0f);
        
        // Apply direction and speed to velocity
        physics_body->velocity.x += physics_body->direction.x * physics_body->speed;
        physics_body->velocity.y += physics_body->direction.y * physics_body->speed;
        
        // Clamp velocity
        float max_velocity = 2.0f; // Example maximum velocity (should be set on the physics_body?)
        if (physics_body->velocity.x > max_velocity) physics_body->velocity.x = max_velocity;
        if (physics_body->velocity.x < -max_velocity) physics_body->velocity.x = -max_velocity;
        if (physics_body->velocity.y > max_velocity) physics_body->velocity.y = max_velocity;
        if (physics_body->velocity.y < -max_velocity) physics_body->velocity.y = -max_velocity;
        
        // Calculate proposed position
        vector2_t proposed_position = { 
            .x = physics_body->position.x + physics_body->velocity.x, 
            .y = physics_body->position.y + physics_body->velocity.y 
        };
        
        // Collision detection
        vector2_t proposed_position_x = (vector2_t){ .x = proposed_position.x, .y = physics_body->position.y };
        vector2_t proposed_position_y = (vector2_t){ .x = physics_body->position.x, .y = proposed_position.y };
        
        // Get tile on all corners of proposed position X and Y
        vector2_t proposed_position_x_tl = (vector2_t){ .x = proposed_position_x.x, .y = proposed_position_x.y };
        vector2_t proposed_position_x_tr = (vector2_t){ .x = proposed_position_x.x + physics_body->aabb.width, .y = proposed_position_x.y };
        vector2_t proposed_position_x_bl = (vector2_t){ .x = proposed_position_x.x, .y = proposed_position_x.y + physics_body->aabb.height };
        vector2_t proposed_position_x_br = (vector2_t){ .x = proposed_position_x.x + physics_body->aabb.width, .y = proposed_position_x.y + physics_body->aabb.height };
        vector2_t proposed_position_y_tl = (vector2_t){ .x = proposed_position_y.x, .y = proposed_position_y.y };
        vector2_t proposed_position_y_tr = (vector2_t){ .x = proposed_position_y.x + physics_body->aabb.width, .y = proposed_position_y.y };
        vector2_t proposed_position_y_bl = (vector2_t){ .x = proposed_position_y.x, .y = proposed_position_y.y + physics_body->aabb.height };
        vector2_t proposed_position_y_br = (vector2_t){ .x = proposed_position_y.x + physics_body->aabb.width, .y = proposed_position_y.y + physics_body->aabb.height };
        
        // Get tile index on all corners of proposed position X and Y
        // TODO: Check out of bounds
        // TODO: Abstract converting to tile index
        int tile_idx_x_tl = level->tilemap[(int)floor(proposed_position_x_tl.y / level->tileset_tile_size)][(int)floor(proposed_position_x_tl.x / level->tileset_tile_size)];
        int tile_idx_x_tr = level->tilemap[(int)floor(proposed_position_x_tr.y / level->tileset_tile_size)][(int)floor(proposed_position_x_tr.x / level->tileset_tile_size)];
        int tile_idx_x_bl = level->tilemap[(int)floor(proposed_position_x_bl.y / level->tileset_tile_size)][(int)floor(proposed_position_x_bl.x / level->tileset_tile_size)];
        int tile_idx_x_br = level->tilemap[(int)floor(proposed_position_x_br.y / level->tileset_tile_size)][(int)floor(proposed_position_x_br.x / level->tileset_tile_size)];
        int tile_idx_y_tl = level->tilemap[(int)floor(proposed_position_y_tl.y / level->tileset_tile_size)][(int)floor(proposed_position_y_tl.x / level->tileset_tile_size)];
        int tile_idx_y_tr = level->tilemap[(int)floor(proposed_position_y_tr.y / level->tileset_tile_size)][(int)floor(proposed_position_y_tr.x / level->tileset_tile_size)];
        int tile_idx_y_bl = level->tilemap[(int)floor(proposed_position_y_bl.y / level->tileset_tile_size)][(int)floor(proposed_position_y_bl.x / level->tileset_tile_size)];
        int tile_idx_y_br = level->tilemap[(int)floor(proposed_position_y_br.y / level->tileset_tile_size)][(int)floor(proposed_position_y_br.x / level->tileset_tile_size)];
        
        // Check for collision on all corners of proposed position X and Y
        bool collision_x_tl = !level->tiledef[tile_idx_x_tl].is_walkable;
        bool collision_x_tr = !level->tiledef[tile_idx_x_tr].is_walkable;
        bool collision_x_bl = !level->tiledef[tile_idx_x_bl].is_walkable;
        bool collision_x_br = !level->tiledef[tile_idx_x_br].is_walkable;
        bool collision_y_tl = !level->tiledef[tile_idx_y_tl].is_walkable;
        bool collision_y_tr = !level->tiledef[tile_idx_y_tr].is_walkable;
        bool collision_y_bl = !level->tiledef[tile_idx_y_bl].is_walkable;
        bool collision_y_br = !level->tiledef[tile_idx_y_br].is_walkable;
        
        // Sum up as axis collision (this will allow for sliding)
        bool collision_x = collision_x_tl || collision_x_tr || collision_x_bl || collision_x_br;
        bool collision_y = collision_y_tl || collision_y_tr || collision_y_bl || collision_y_br;
        
        // Collision response
        if (entity->type == ENTITY_TYPE_PLAYER) {
            if (collision_x) {
                physics_body->velocity.x = -physics_body->velocity.x; // Invert velocity
            } else {
                physics_body->position.x = proposed_position.x;
            }
            
            if (collision_y) {
                physics_body->velocity.y = -physics_body->velocity.y; // Invert velocity
            } else {
                physics_body->position.y = proposed_position.y;
            }
        } else {
            switch(entity->npc.behaviour) {
                case ENTITY_BEHAVIOUR_PATROL: 
                    // TODO: update to more complex behaviour
                    if (collision_x) {
                        physics_body->direction.x = -physics_body->direction.x; // Invert direction
                        physics_body->velocity.x = -physics_body->velocity.x; // Invert velocity
                    } else {
                        physics_body->position.x = proposed_position.x;
                    }
                    if (collision_y) {
                        physics_body->direction.y = -physics_body->direction.y; // Invert direction
                        physics_body->velocity.y = -physics_body->velocity.y; // Invert velocity
                    } else {
                        physics_body->position.y = proposed_position.y;
                    }
                    break;
                default:
                case ENTITY_BEHAVIOUR_STATIONARY:
                    break;
            }
        }
        
        // Update AABB position
        physics_body->aabb.x = physics_body->position.x;
        physics_body->aabb.y = physics_body->position.y;
    }
}

