#ifndef RENDERER_H
#define RENDERER_H

#include <raylib.h>

#include "map1.h"
#include "player.h"
#include "textures.h"

#define SCREEN_WIDTH 1920
#define SCREEN_HEIGHT 1080

#define SCREEN_CENTER                                                          \
    (Vector2) { SCREEN_WIDTH / 2.f, SCREEN_HEIGHT / 2.f }
#define WALL_HEIGHT 900.f

#define TILE_SIZE ((float)SCREEN_WIDTH / MAP_WIDTH)

void draw_world(Player*, TextureArr*);
void draw_everything(Player*, TextureArr*);

#endif
