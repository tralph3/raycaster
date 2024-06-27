#ifndef RENDERER_H
#define RENDERER_H

#include <raylib.h>

#include "map.h"
#include "player.h"
#include "textures.h"
#include "game.h"

#define FACTOR 120
#define SCREEN_WIDTH (16*FACTOR)
#define SCREEN_HEIGHT (9*FACTOR)

void print_vector(Vector2);
void sort_sprites(Player*, SpriteArr*);
void draw_world(Game*);
void draw_everything(Game*);

#endif
