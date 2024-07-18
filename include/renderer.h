#ifndef RENDERER_H
#define RENDERER_H

#include <raylib.h>

#include "map.h"
#include "player.h"
#include "textures.h"

typedef struct {
  int min;
  int max;
} RayPair;

typedef struct {
  int render_width;
  int render_height;
  int screen_width;
  int screen_height;
  TextureArr textures;
  SpriteArr sprites;
  Texture2D render_texture;
  Color *screen_buffer;
} Renderer;

void sort_sprites(Player*, SpriteArr*);
void *draw_stripe(void*);
void draw_world(Renderer *, Player *, Map *);
void draw_everything(Renderer *, Player *, Map *);
void draw_sprites(Renderer *, Camera *);
void init_ray_lengths(Renderer *);
void draw_pixel(Renderer *, int, int, Color, Color);

#endif
