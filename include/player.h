#ifndef PLAYER_H
#define PLAYER_H

#include <raylib.h>
#include <raymath.h>

typedef struct {
    Vector2 position;
    Vector2 old_position;
    Vector2 direction;
    Vector2 camera_plane;
    float speed;
    float size;
    float rotation_speed;
    Camera camera;
    float pitch;
} Player;

typedef enum {
  PLAYER_DIRECTION_LEFT = 1,
  PLAYER_DIRECTION_RIGHT,
  PLAYER_DIRECTION_UP,
  PLAYER_DIRECTION_DOWN,
} PlayerDirection;

void player_move(Player*, Vector2, float);
void player_rotate(Player*, float);
void player_pitch(Player*, float);
Player create_new_player(Vector2, PlayerDirection);
void update_player_camera(Player*);

#endif
