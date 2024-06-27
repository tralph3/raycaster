#ifndef RENDERER_H
#define RENDERER_H

#include <raylib.h>

#include "map.h"
#include "player.h"
#include "textures.h"
#include "game.h"

#define FACTOR 120
#define SCREEN_WIDTH (16*FACTOR)
#define SCREEN_HEIGHT (9 * FACTOR)

typedef struct {
  int render_width;
  int render_height;
  TextureArr textures;
} Renderer;

void print_vector(Vector2);
void sort_sprites(Player*, SpriteArr*);
void calculate_visible_walls(Game*);
void draw_everything(Game*);
void draw_sprites(Game *);
Vector2 cast_ray(int, Player *, Map *);
void draw_floor_cell(Vector2, Game *);
void draw_ceiling_cell(Vector2, Game *);
void draw_wall_cell(Vector2, Game*);

#endif
