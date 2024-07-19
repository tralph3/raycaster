#include "renderer.h"
#include "map.h"
#include "textures.h"
#include <raylib.h>
#include <raymath.h>
#include <rlgl.h>
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>

#define RENDER_THREAD_COUNT 8

typedef enum {
  SIDE_RIGHT = 0,
  SIDE_LEFT,
} Side;

typedef struct {
  Player *player;
  Renderer *renderer;
  Map *map;
  int start;
  int end;
} DrawStripeArgs;

float *ray_lengths;

pthread_t threads[RENDER_THREAD_COUNT] = {0};
DrawStripeArgs thread_args[RENDER_THREAD_COUNT] = {0};

inline void draw_pixel(Renderer *renderer, int x, int y, Color color, Color tint) {
  color = ColorTint(color, tint);
  renderer->screen_buffer[x + y * renderer->render_width] = color;
}

void init_ray_lengths(Renderer *renderer) {
  ray_lengths = malloc(renderer->render_height * sizeof(float));
  for (int y = renderer->render_height / 2.f; y < renderer->render_height; ++y) {
    float ray_angle = atan(y - renderer->render_height / 2.f);
    ray_lengths[y] = (renderer->render_height / 2.f) / tan(ray_angle);
  }
}

void *draw_stripe(void *void_args) {
  DrawStripeArgs *args = void_args;
  Player *player = args->player;
  Renderer *renderer = args->renderer;
  Map *map = args->map;
  int start = args->start;
  int end = args->end;

  for (int x = start; x < end; ++x) {
    Vector2 map_pos = (Vector2){(int)player->position.x, (int)player->position.y};
    float p_percentage_x = player->position.x - map_pos.x;
    float p_percentage_y = player->position.y - map_pos.y;
    int step_x;
    int step_y;
    bool hit = false;
    double wall_x;
    int side;
    float perpendicular_wall_dist;
    MapTile wall_tile;
    double camera_x = (x << 1) / (double)renderer->render_width - 1;
    Vector2 ray_dir = Vector2Add(
                                 player->direction, Vector2Scale(player->camera_plane, camera_x));

    Vector2 delta_dist = {
      .x = ray_dir.x == 0 ? 1e30 : fabs(1 / ray_dir.x),
      .y = ray_dir.y == 0 ? 1e30 : fabs(1 / ray_dir.y),
    };
    Vector2 side_dist;
    if (ray_dir.x < 0) {
      step_x = -1;
      side_dist.x = p_percentage_x * delta_dist.x;
    } else {
      step_x = 1;
      side_dist.x = (1 - p_percentage_x) * delta_dist.x;
    }
    if (ray_dir.y < 0) {
      step_y = -1;
      side_dist.y = p_percentage_y * delta_dist.y;
    } else {
      step_y = 1;
      side_dist.y = (1 - p_percentage_y) * delta_dist.y;
    }

    while (!hit) {
      if (side_dist.x < side_dist.y) {
        side_dist.x += delta_dist.x;
        map_pos.x += step_x;
        side = SIDE_RIGHT;
      } else {
        side_dist.y += delta_dist.y;
        map_pos.y += step_y;
        side = SIDE_LEFT;
      }
      wall_tile = get_tile_at_point(map, map_pos);
      if (wall_tile.type == TILE_TYPE_WALL)
        hit = true;
    }

    switch (side) {
    case SIDE_RIGHT:
      perpendicular_wall_dist = side_dist.x - delta_dist.x;
      wall_x = player->position.y + perpendicular_wall_dist * ray_dir.y;
      break;
    case SIDE_LEFT:
      perpendicular_wall_dist = side_dist.y - delta_dist.y;
      wall_x = player->position.x + perpendicular_wall_dist * ray_dir.x;
      break;
    default:
      exit(1);
      break;
    }

    float line_height = renderer->render_height / perpendicular_wall_dist;
    float wall_bottom = renderer->render_height / 2.f + line_height / 2.f;

    for (int y = wall_bottom; y < renderer->render_height; ++y) {
      Vector2 texture_pos =
        Vector2Add(player->position, Vector2Scale(ray_dir, ray_lengths[y]));
      MapTile tile = get_tile_at_point(map, texture_pos);
      TexturePixels floor_texture = get_texture(&renderer->textures, tile.floor_id);
      TexturePixels ceiling_texture = get_texture(&renderer->textures, tile.ceiling_id);
      texture_pos.x -= (int)texture_pos.x;
      texture_pos.y -= (int)texture_pos.y;

      int floor_tex_x = texture_pos.x * floor_texture.texture.width;
      int floor_tex_y = texture_pos.y * floor_texture.texture.height;
      int ceiling_tex_x = texture_pos.x * ceiling_texture.texture.width;
      int ceiling_tex_y = texture_pos.y * ceiling_texture.texture.height;
      draw_pixel(renderer, x, y, get_texture_pixel(floor_texture, floor_tex_x, floor_tex_y), WHITE);
      int inverted_y = fmaxf(0, y - ((y + 1 -(renderer->render_height/2.f)) * 2));
      draw_pixel(renderer, x, inverted_y, get_texture_pixel(ceiling_texture, ceiling_tex_x, ceiling_tex_y), WHITE);
    }

    wall_x -= (int)wall_x;
    TexturePixels wall_texture = get_texture(&renderer->textures, wall_tile.wall_id);
    int tex_x = wall_x * wall_texture.texture.width;
    if (ray_dir.x < 0 && side == SIDE_RIGHT)
      tex_x = wall_texture.texture.width - tex_x - 1;
    if (ray_dir.y > 0 && side == SIDE_LEFT)
      tex_x = wall_texture.texture.width - tex_x - 1;

    float draw_start = -line_height / 2.f + renderer->render_height / 2.f;
    int y_start = fmaxf(0, draw_start);
    int y_end = fminf(draw_start + line_height, renderer->render_height);
    for (int y = y_start; y < y_end; ++y) {
      float y_percentage = (y - draw_start) / line_height;
      int tex_y = fmaxf(0, y_percentage * wall_texture.texture.height);
      Color tint = WHITE;
      if (side == SIDE_LEFT) tint = ColorBrightness(tint, -0.2f);
      draw_pixel(renderer, x, y, get_texture_pixel(wall_texture, tex_x, tex_y), tint);
    }
  }
  pthread_exit(NULL);
  return NULL;
}


void draw_world(Renderer *renderer, Player *player, Map *map) {
  int offset = renderer->render_width / RENDER_THREAD_COUNT;
  int leftover = renderer->render_width - (offset * RENDER_THREAD_COUNT);
  pthread_t leftover_thread;
  if (leftover > 0) {
    DrawStripeArgs args = {
      .renderer = renderer,
      .map = map,
      .player = player,
      .start = renderer->render_width - leftover,
      .end = renderer->render_width,
    };
    pthread_create(&leftover_thread, NULL, draw_stripe, &args);
  }
    for (int i = 0; i < RENDER_THREAD_COUNT; ++i) {
      thread_args[i].renderer = renderer;
      thread_args[i].map = map;
      thread_args[i].player = player;
      thread_args[i].start = offset * i;
      thread_args[i].end = offset * (i + 1);
      pthread_create(threads + i, NULL, draw_stripe, thread_args + i);
    }
    for (int i = 0; i < RENDER_THREAD_COUNT; ++i)
      pthread_join(threads[i], NULL);
    if (leftover > 0)
      pthread_join(leftover_thread, NULL);
}

void draw_everything(Renderer *renderer, Player *player, Map *map) {
  BeginDrawing();
  ClearBackground(BLACK);
  draw_world(renderer, player, map);
  UpdateTexture(renderer->render_texture, renderer->screen_buffer);
  DrawTexturePro(renderer->render_texture, (Rectangle){0, 0, renderer->render_width, renderer->render_height}, (Rectangle){0, 0, renderer->screen_width, renderer->screen_height}, Vector2Zero(), 0, WHITE);
  DrawFPS(0, 0);
  EndDrawing();
}

void sort_sprites(Player *player, SpriteArr *sprites) {
  bool swapped;
  for (int i = 0; i < sprites->count - 1; ++i) {
    swapped = false;
    for (int j = 0; j < sprites->count - i - 1; ++j) {
      Vector2 sprite1_relative_pos = Vector2Subtract(player->position, sprites->items[j].position);
      Vector2 sprite2_relative_pos = Vector2Subtract(player->position, sprites->items[j + 1].position);
      if (Vector2LengthSqr(sprite1_relative_pos) <
          Vector2LengthSqr(sprite2_relative_pos)){
        Sprite aux = sprites->items[j];
        sprites->items[j] = sprites->items[j + 1];
        sprites->items[j + 1] = aux;
        swapped = true;
      }
    }
    if (!swapped)
      break;
  }
}
