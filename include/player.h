#ifndef PLAYER_H
#define PLAYER_H

#include <raylib.h>
#include "vector.h"

#define PLAYER_SIZE 10

typedef struct {
    Vector2 position;
    Vector2 oldPosition;
    Vector2 direction;
    Vector2 cameraPlane;
} Player;

void player_move(Player *player, float amount) {
    player->oldPosition = player->position;
    player->position =
        vec2_add(player->position, vec2_mul(player->direction, amount));
}

void player_rotate(Player *player, float amount) {
    player->direction = vec2_rotate(player->direction, amount);
    player->cameraPlane = vec2_rotate(player->cameraPlane, amount);
}

void player_strafe(Player *player, float amount) {
    player->oldPosition = player->position;
    player->position = vec2_add(player->position, vec2_mul(player->cameraPlane, amount));
}

#endif
