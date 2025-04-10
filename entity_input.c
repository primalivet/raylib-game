#include "entity.h"

void input_update(entity_input_comp_t *input) {
  input->space_prev = input->space;
  input->up    = IsKeyDown(KEY_UP)    || IsKeyDown(KEY_W);
  input->down  = IsKeyDown(KEY_DOWN)  || IsKeyDown(KEY_S);
  input->left  = IsKeyDown(KEY_LEFT)  || IsKeyDown(KEY_A);
  input->right = IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D);
  input->space  = IsKeyDown(KEY_SPACE);
}
