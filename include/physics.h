#ifndef PHYSICS_H
#define PHYSICS_H

#include "player.h"
#include "map.h"

#include <stdbool.h>

void check_collission(Player*, Map*);
bool is_square_colliding(Vector2, float, Map*);

#endif
