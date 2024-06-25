#ifndef RENDERER_H
#define RENDERER_H

#include <raylib.h>

#include "map.h"
#include "player.h"
#include "textures.h"

#define SCREEN_WIDTH 1920
#define SCREEN_HEIGHT 1080

#define WALL_HEIGHT (SCREEN_HEIGHT * 0.85f)

void print_vector(Vector2);
void sort_sprites(Player*, SpriteArr*);
void draw_world(Player*, TextureArr*, SpriteArr*, Map*);
void draw_everything(Player*, TextureArr*, SpriteArr*, Map*);
void init_ray_lengths(void);

#endif
