#ifndef RENDERER_H
#define RENDERER_H

#include <raylib.h>

#include "map.h"
#include "player.h"
#include "textures.h"

typedef struct {
  int render_width;
  int render_height;
  int screen_width;
  int screen_height;
  TextureArr textures;
  SpriteArr sprites;
} Renderer;

void print_vector(Vector2);
void sort_sprites(Player*, SpriteArr*);
void calculate_visible_walls(Renderer *, Player *, Map *);
void draw_everything(Renderer *, Player *, Map *);
void draw_sprites(Renderer *, Camera *);
Vector2 cast_ray(Renderer*, int, Player *, Map *);
void draw_floor_cell(Vector2, Renderer *, Map *);
void draw_ceiling_cell(Vector2, Renderer *, Map *);
void draw_wall_cell(Vector2, Renderer *, Map *);

#endif
