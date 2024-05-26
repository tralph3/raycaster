#ifndef PHYSICS_H
#define PHYSICS_H

#include "player.h"
#include "renderer.h"

void check_collission(Player *player) {
    int map_x = player->position.x / tile_size;
    int map_y = player->position.y / tile_size;
    int tile = world_map[map_x + map_y * MAP_WIDTH];
    if (tile > 0)
        player->position = player->oldPosition;
}

#endif
