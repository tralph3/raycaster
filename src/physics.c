#include "renderer.h"
#include "player.h"

void check_collission(Player *player) {
    int map_x = player->position.x / TILE_SIZE;
    int map_y = player->position.y / TILE_SIZE;
    int tile = world_map[map_x + map_y * MAP_WIDTH];
    if (tile > 0)
        player->position = player->oldPosition;
}
