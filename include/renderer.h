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
  Texture2D buffer_texture;
  Color *screen_buffer;
  RenderTexture2D render_texture;
} Renderer;

void sort_sprites(Player*, SpriteArr*);
void *draw_stripe(void*);
void draw_world(Renderer *, Player *, Map *);
void draw_everything(Renderer *, Player *, Map *);
void draw_sprites(Renderer *, Player *);
void init_ray_lengths(Renderer *);
void draw_pixel(Renderer *, int, int, Color, Color);
void init_renderer(Renderer *);
void set_render_resolution(Renderer *, unsigned int, unsigned int);

#endif
