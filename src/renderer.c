#include "renderer.h"
#include "map.h"
#include "textures.h"
#include <math.h>
#include <raylib.h>
#include <raymath.h>
#include <rlgl.h>
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/sysinfo.h>

int render_thread_count;

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

typedef struct {
  Vector2 position;
  float intensity;
  float fall_off;
} LightSource;
float natural_light = 0.1f;

LightSource light_source = {
  {1, 1},
  0.01f,
  10.f,
};

pthread_t *threads;
DrawStripeArgs *thread_args;

void init_renderer(Renderer *renderer) {
  render_thread_count = get_nprocs();
  threads = malloc(render_thread_count * sizeof(pthread_t));
  thread_args = malloc(render_thread_count * sizeof(DrawStripeArgs));

  int current_monitor = GetCurrentMonitor();
  renderer->render_width = GetMonitorWidth(current_monitor);
  renderer->render_height = GetMonitorHeight(current_monitor);
  renderer->screen_width = GetMonitorWidth(current_monitor);
  renderer->screen_height = GetMonitorHeight(current_monitor);
  renderer->render_width = 640;
  renderer->render_height = 480;
  Texture2D render_texture = LoadRenderTexture(renderer->render_height, renderer->render_width).texture;
  renderer->render_texture = render_texture;
  renderer->screen_buffer = malloc(renderer->render_width * renderer->render_height * sizeof(Color));
  TextureArr textures = load_all_textures();
  renderer->textures = textures;
  SpriteArr sprites = {0};
  renderer->sprites = sprites;
}

inline void draw_pixel(Renderer *renderer, int x, int y, Color color, Color tint) {
  color = ColorTint(color, tint);
  renderer->screen_buffer[y + x * renderer->render_height] = color;
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
    float wall_x;
    int side;
    float perpendicular_wall_dist;
    MapTile wall_tile;
    float camera_x = (x << 1) / (double)renderer->render_width - 1;
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
      switch (wall_tile.type) {
      /* case TILE_TYPE_THIN_WALL: */
      /*   break; */
      case TILE_TYPE_WALL: {
        hit = true;

      } break;
      default:
        break;
      }
    }

    Vector2 wall_stripe_position;
    switch (side) {
    case SIDE_RIGHT:
      perpendicular_wall_dist = side_dist.x - delta_dist.x;
      wall_stripe_position = Vector2Add(player->position, Vector2Scale(ray_dir, perpendicular_wall_dist));
      wall_x = wall_stripe_position.y;
      break;
    case SIDE_LEFT:
      perpendicular_wall_dist = side_dist.y - delta_dist.y;
      wall_stripe_position = Vector2Add(player->position, Vector2Scale(ray_dir, perpendicular_wall_dist));
      wall_x = wall_stripe_position.x;
      break;
    default:
      exit(1);
      break;
    }

    float line_height = renderer->render_height / perpendicular_wall_dist;
    float wall_top = renderer->render_height / 2.f - line_height / 2.f;
    line_height += 4; // + 4 for rounding error

    for (int y = 0; y < wall_top; ++y) {
      float ray_length = (renderer->render_height / 2.f) / (renderer->render_height / 2.f -  y);
      Vector2 texture_pos =
        Vector2Add(player->position, Vector2Scale(ray_dir, ray_length));
      MapTile tile = get_tile_at_point(map, texture_pos);
      TexturePixels floor_texture = get_texture(&renderer->textures, tile.floor_id);
      TexturePixels ceiling_texture = get_texture(&renderer->textures, tile.ceiling_id);
      float tile_light_source_distance = Vector2LengthSqr(Vector2Subtract(texture_pos, light_source.position));;
      texture_pos.x -= (int)texture_pos.x;
      texture_pos.y -= (int)texture_pos.y;

      int floor_tex_x = texture_pos.x * floor_texture.texture.width;
      int floor_tex_y = texture_pos.y * floor_texture.texture.height;
      int ceiling_tex_x = texture_pos.x * ceiling_texture.texture.width;
      int ceiling_tex_y = texture_pos.y * ceiling_texture.texture.height;
      float distance_to_screen_center = renderer->render_height / 2.f - y;
      int mirrored_y = y + distance_to_screen_center * 2;
      Color tint = WHITE;
      tint = ColorBrightness(tint, -tile_light_source_distance*light_source.intensity);
      tint = ColorBrightness(tint, natural_light);
      draw_pixel(renderer, x, y, get_texture_pixel(ceiling_texture, ceiling_tex_x, ceiling_tex_y), tint);
      draw_pixel(renderer, x, mirrored_y, get_texture_pixel(floor_texture, floor_tex_x, floor_tex_y), tint);
    }

    wall_x -= (int)wall_x;
    float light_source_distance = Vector2LengthSqr(Vector2Subtract(wall_stripe_position, light_source.position));
    TexturePixels wall_texture = get_texture(&renderer->textures, wall_tile.wall_id);
    int tex_x = wall_x * wall_texture.texture.width;
    if ((ray_dir.x < 0 && side == SIDE_RIGHT) || (ray_dir.y > 0 && side == SIDE_LEFT))
      tex_x = wall_texture.texture.width - tex_x - 1;

    float draw_start = -line_height / 2.f + renderer->render_height / 2.f - 1; // - 1 for rounding error
    int y_start = fmaxf(0, draw_start);
    int y_end = fminf(draw_start + line_height, renderer->render_height);
    Color tint = WHITE;
    tint = ColorBrightness(tint, -light_source_distance*light_source.intensity);
    tint = ColorBrightness(tint, natural_light);
    if (side == SIDE_LEFT) tint = ColorBrightness(tint, -0.2f);
    for (int y = y_start; y < y_end; ++y) {
      float y_percentage = (y - draw_start) / line_height;
      int tex_y = fmaxf(0, y_percentage * wall_texture.texture.height);
      draw_pixel(renderer, x, y, get_texture_pixel(wall_texture, tex_x, tex_y), tint);
    }
  }
  pthread_exit(NULL);
  return NULL;
}

void draw_world(Renderer *renderer, Player *player, Map *map) {
  int offset = renderer->render_width / render_thread_count;
  int leftover = renderer->render_width - (offset * render_thread_count);
  for (int i = 0; i < render_thread_count; ++i) {
    thread_args[i].renderer = renderer;
    thread_args[i].map = map;
    thread_args[i].player = player;
    thread_args[i].start = offset * i;
    if (leftover > 0 && i == render_thread_count - 1)
      thread_args[i].end = offset * i + leftover;
    else
      thread_args[i].end = offset * (i + 1);
    pthread_create(threads + i, NULL, draw_stripe, thread_args + i);
  }
  for (int i = 0; i < render_thread_count; ++i)
    pthread_join(threads[i], NULL);

  UpdateTexture(renderer->render_texture, renderer->screen_buffer);
  DrawTexturePro(renderer->render_texture, (Rectangle){0, 0, renderer->render_height, -renderer->render_width}, (Rectangle){0, 0, renderer->screen_height, renderer->screen_width}, (Vector2){0,renderer->screen_width}, 90, WHITE);
}

void draw_skybox(Renderer *renderer, Player *player, Map *map) {
  Texture2D skybox_texture = get_texture(&renderer->textures, map->skybox).texture;
  float player_angle = atan2(player->direction.y, player->direction.x);
  if (player_angle < 0) player_angle += 2*PI;
  float angle_percentage = player_angle / (2*PI);
  DrawTexturePro(skybox_texture,
                 (Rectangle){angle_percentage * skybox_texture.width, 0, skybox_texture.width/4.f, skybox_texture.height},
                 (Rectangle){0, 0, renderer->screen_width, renderer->screen_height/2.f},
                 Vector2Zero(), 0, WHITE);
}

void draw_everything(Renderer *renderer, Player *player, Map *map) {
  BeginDrawing();
  ClearBackground(BLACK);
  draw_skybox(renderer, player, map);
  draw_world(renderer, player, map);
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
