#include "level.h"
#include "input.h"
#include "entity.h"

#ifndef ENTITY_PHYSICS_H
#define ENTITY_PHYSICS_H

void physics_update(entity_physics_comp_t *physics_body, level_t *level,  entity_input_comp_t *input);
#endif
