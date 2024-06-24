#ifndef PLAYER_H
#define PLAYER_H

#include <raymath.h>

typedef struct {
    Vector2 position;
    Vector2 oldPosition;
    Vector2 direction;
    Vector2 cameraPlane;
    float speed;
    float size;
    float rotation_speed;
} Player;

void player_move(Player*, Vector2, float);
void player_rotate(Player*, float);

#endif
