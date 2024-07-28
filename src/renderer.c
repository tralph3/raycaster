#include "renderer.h"
#include "map.h"
#include "textures.h"
#include "dynarray.h"
#include <math.h>
#include <raylib.h>
#include <raymath.h>
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <physics.h>
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
} DrawSectionArgs;

typedef struct {
  Vector2 position;
  float intensity;
  float cutoff;
  Color tint;
} LightSource;

float natural_light = 1.f;

float *z_buffer = NULL;
pthread_t *threads = NULL;
DrawSectionArgs *thread_args = NULL;

LightSource light_sources[5] = {0};

bool has_direct_path(Map *map, Vector2 *start, Vector2 *end) {
  Vector2 ray_dir = Vector2Normalize(Vector2Subtract(*end, *start));
  *end = Vector2Add(*end, Vector2Scale(ray_dir, -0.0001f));
  Vector2 map_pos = (Vector2){(int)start->x, (int)start->y};
  Vector2 end_map_pos = (Vector2){(int)end->x, (int)end->y};
  float p_percentage_x = start->x - map_pos.x;
  float p_percentage_y = start->y - map_pos.y;
  int step_x;
  int step_y;
  bool hit = false;

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
    if(map_pos.x == end_map_pos.x && map_pos.y == end_map_pos.y)
      break;
    if (side_dist.x < side_dist.y) {
      side_dist.x += delta_dist.x;
      map_pos.x += step_x;
    } else {
      side_dist.y += delta_dist.y;
      map_pos.y += step_y;
    }

    MapTile *tile;
    GET_TILE_AT_POINT_DEFAULT_WALL(tile, map, &map_pos);
    switch (tile->type) {
      /* case TILE_TYPE_THIN_WALL: */
      /*   break; */
    case TILE_TYPE_WALL: {
      return false;
    } break;
    default:
      break;
    }
  }
  return true;
}

Color get_point_light_value(Vector2 *position, Map *map) {
  float max_factor = -1;
  Color tint = WHITE;
  for (int i = 0; i < 0; ++i) {
    LightSource light_source = light_sources[i];
    float light_source_distance = Vector2Length(Vector2Subtract(*position, light_source.position));
    if (light_source_distance > light_source.cutoff) continue;
    if (!has_direct_path(map, &light_source.position, position)) continue;
    float factor = -light_source_distance / light_source.intensity;
    if (factor > max_factor) {
      max_factor = factor;
      tint = light_source.tint;
    }
  }
  tint = ColorBrightness(tint, max_factor);

  MapTile *tile;
  GET_TILE_AT_POINT_DEFAULT_WALL(tile, map, position);
  if (tile->ceiling_id == 0 && tile->type != TILE_TYPE_WALL)
    tint = ColorBrightness(tint, natural_light + 0.2);
  else
    tint = ColorBrightness(tint, natural_light);
  return tint;
}

void init_renderer(Renderer *renderer) {
  render_thread_count = get_nprocs();
  /* render_thread_count = 1; */
  threads = malloc(render_thread_count * sizeof(pthread_t));
  thread_args = malloc(render_thread_count * sizeof(DrawSectionArgs));

  int current_monitor = GetCurrentMonitor();
  renderer->screen_width = GetMonitorWidth(current_monitor);
  renderer->screen_height = GetMonitorHeight(current_monitor);

  set_render_resolution(renderer, GetMonitorWidth(current_monitor), GetMonitorHeight(current_monitor));
  set_render_resolution(renderer, 640, 480);
  TextureArr textures = load_all_textures();
  renderer->textures = textures;
  SpriteArr sprites = {0};
  Sprite sprite = {
    .position = {3.5, 3.5},
    .texture_id = 13,
  };
  da_append(&sprites, sprite);
  renderer->sprites = sprites;
}

void set_render_resolution(Renderer *renderer, unsigned int width, unsigned int height) {
  renderer->render_width = width;
  renderer->render_height = height;

  // for screen_buffer drawing
  UnloadTexture(renderer->buffer_texture);
  renderer->buffer_texture = LoadRenderTexture(renderer->render_height, renderer->render_width).texture;

  // for built-in draw functions
  UnloadRenderTexture(renderer->render_texture);
  renderer->render_texture = LoadRenderTexture(renderer->render_width, renderer->render_height);

  free(renderer->screen_buffer);
  renderer->screen_buffer = malloc(renderer->render_width * renderer->render_height * sizeof(Color));
  if (renderer->screen_buffer == NULL) {
    fprintf(stderr, "ERROR: Could not allocate memory for screen_buffer\n");
    exit(1);
  }

  free(z_buffer);
  z_buffer = malloc(renderer->render_width * sizeof(float));
  if (z_buffer == NULL) {
    fprintf(stderr, "ERROR: Could not allocate memory for z_buffer\n");
    exit(1);
  }
}

inline void draw_pixel(Renderer *renderer, int x, int y, Color *color, Color *tint) {
  *color = ColorTint(*color, *tint);
  renderer->screen_buffer[y + x * renderer->render_height] = *color;
}

void *draw_section(void *void_args) {
  DrawSectionArgs *args = void_args;
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
    MapTile *wall_tile;
    float camera_x = (x << 1) / (float)renderer->render_width - 1;
    Vector2 ray_dir = Vector2Add(player->direction, Vector2Scale(player->camera_plane, camera_x));

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

      GET_TILE_AT_POINT_DEFAULT_WALL(wall_tile, map, &map_pos);
      switch (wall_tile->type) {
      case TILE_TYPE_PLANE_WALL: {
        Vector2 start = Vector2Add(((PlaneWallArgs*)wall_tile->args)->start, map_pos);
        Vector2 end = Vector2Add(((PlaneWallArgs*)wall_tile->args)->end, map_pos);
        Vector2 intersection;
        if (CheckCollisionLines(player->position, Vector2Add(player->position, Vector2Scale(ray_dir, 1000)), start, end, &intersection)) {
          hit = true;
          side = fabsf(start.y - end.y) < 0.4f;
          if (side == SIDE_RIGHT) {
            if (ray_dir.x < 0) {
              side_dist.x += delta_dist.x * (1 - (intersection.x - (int)intersection.x));
            } else {
              side_dist.x += delta_dist.x * (intersection.x - (int)intersection.x);
            }
          } else {
            if (ray_dir.y < 0) {
              side_dist.y += delta_dist.y * (1 - (intersection.y - (int)intersection.y));
            } else {
              side_dist.y += delta_dist.y * (intersection.y - (int)intersection.y);
            }
          }
        }
      } break;
      case TILE_TYPE_WALL:
        hit = true;
        break;
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

    z_buffer[x] = perpendicular_wall_dist;
    float line_height = renderer->render_height / perpendicular_wall_dist;
    float wall_top = renderer->render_height * player->plane_height - line_height / 2.f;
    wall_top -= (0.5 - player->height) * line_height;
    float wall_bot = wall_top + line_height;

    wall_x -= (int)wall_x;
    TexturePixels wall_texture = get_texture(&renderer->textures, wall_tile->wall_id);
    int tex_x = wall_x * wall_texture.texture.width;
    if (((ray_dir.x < 0 && side == SIDE_RIGHT) || (ray_dir.y > 0 && side == SIDE_LEFT)) && wall_tile->type != TILE_TYPE_PLANE_WALL)
      tex_x = wall_texture.texture.width - tex_x - 1;

    float center = renderer->render_height * player->plane_height;
    float height = renderer->render_height * (1 - player->height);
    for (int y = 0; y < wall_top; ++y) {
      float ray_length =  height / (center - y);
      Vector2 texture_pos =
        Vector2Add(player->position, Vector2Scale(ray_dir, ray_length));
      MapTile *tile;
      GET_TILE_AT_POINT_DEFAULT_WALL(tile, map, &texture_pos);
      Color tint = get_point_light_value(&texture_pos, map);

      TexturePixels ceiling_texture = get_texture(&renderer->textures, tile->ceiling_id);
      texture_pos.x -= (int)texture_pos.x;
      texture_pos.y -= (int)texture_pos.y;

      int ceiling_tex_x = texture_pos.x * (ceiling_texture.texture.width - 1);
      int ceiling_tex_y = texture_pos.y * (ceiling_texture.texture.height - 1);
      Color color = get_texture_pixel(ceiling_texture, ceiling_tex_x, ceiling_tex_y);
      draw_pixel(renderer, x, y, &color, &tint);
    }

    int y_start = wall_top < 0 ? 0 : wall_top;
    int y_end = wall_bot > renderer->render_height ? renderer->render_height : wall_bot;
    Color tint = get_point_light_value(&wall_stripe_position, map);
    tint = ColorBrightness(tint, -0.2f * side);
    for (int y = y_start; y <= y_end; ++y) {
      float y_percentage = Clamp((y - (int)wall_top) / line_height, 0, 1.0);
      int tex_y = y_percentage * (wall_texture.texture.height - 1);
      Color color = get_texture_pixel(wall_texture, tex_x, tex_y);
      draw_pixel(renderer, x, y, &color, &tint);
    }

    height = renderer->render_height * player->height;
    for (int y = renderer->render_height; y > wall_bot; --y) {
      float ray_length = height / (y - center);
      Vector2 texture_pos =
        Vector2Add(player->position, Vector2Scale(ray_dir, ray_length));
      MapTile *tile;
      GET_TILE_AT_POINT_DEFAULT_WALL(tile, map, &texture_pos);
      Color tint = get_point_light_value(&texture_pos, map);

      TexturePixels floor_texture = get_texture(&renderer->textures, tile->floor_id);
      texture_pos.x -= (int)texture_pos.x;
      texture_pos.y -= (int)texture_pos.y;

      int floor_tex_x = texture_pos.x * (floor_texture.texture.width - 1);
      int floor_tex_y = texture_pos.y * (floor_texture.texture.height - 1);

      Color color = get_texture_pixel(floor_texture, floor_tex_x, floor_tex_y);
      draw_pixel(renderer, x, y, &color, &tint);
    }
  }
  pthread_exit(NULL);
  return NULL;
}

void draw_world(Renderer *renderer, Player *player, Map *map) {
  div_t offset = div(renderer->render_width, render_thread_count);
  for (int i = 0; i < render_thread_count; ++i) {
    thread_args[i].renderer = renderer;
    thread_args[i].map = map;
    thread_args[i].player = player;
    thread_args[i].start = offset.quot * i;
    thread_args[i].end = offset.quot * (i + 1) + offset.rem * (i == render_thread_count - 1);
    pthread_create(threads + i, NULL, draw_section, thread_args + i);
  }
  for (int i = 0; i < render_thread_count; ++i)
    pthread_join(threads[i], NULL);

  UpdateTexture(renderer->buffer_texture, renderer->screen_buffer);
  DrawTexturePro(renderer->buffer_texture, (Rectangle){0, 0, renderer->render_height, -renderer->render_width}, (Rectangle){0, 0, renderer->render_height, renderer->render_width}, (Vector2){0,renderer->render_width}, 90, WHITE);
}

void draw_skybox(Renderer *renderer, Player *player, Map *map) {
  Texture2D skybox_texture = get_texture(&renderer->textures, map->skybox).texture;
  float player_angle = atan2(player->direction.y, player->direction.x);
  if (player_angle < 0) player_angle += 2*PI;
  float angle_percentage = player_angle / (2*PI);
  DrawTexturePro(skybox_texture,
                 (Rectangle){angle_percentage * skybox_texture.width, 0, skybox_texture.width/4.f, skybox_texture.height},
                 (Rectangle){0, 0, renderer->render_width, renderer->render_height * player->plane_height},
                 Vector2Zero(), 0, WHITE);
}

void draw_sprites(Renderer *renderer, Player *player) {
  sort_sprites(player, &renderer->sprites);
  for (int i = 0; i < renderer->sprites.count; ++i) {
    Sprite sprite = renderer->sprites.items[i];
    Vector2 sprite_pos =
      Vector2Subtract(renderer->sprites.items[i].position, player->position);
    TexturePixels sprite_texture = get_texture(&renderer->textures, sprite.texture_id);
    float inverse_determinant = 1.0f / (player->camera_plane.x * player->direction.y - player->direction.x * player->camera_plane.y);

    float transform_x = inverse_determinant * (player->direction.y * sprite_pos.x - player->direction.x * sprite_pos.y);
    float transform_y = inverse_determinant * (-player->camera_plane.y * sprite_pos.x + player->camera_plane.x * sprite_pos.y);

    int sprite_screen_x = (renderer->render_width / 2.f) * (1 + transform_x / transform_y);

    int sprite_height = fabs(renderer->render_height / transform_y);
    int sprite_width = fabs( renderer->render_height / transform_y);

    int draw_start_x = -sprite_width / 2.f + sprite_screen_x;
    if(draw_start_x < 0) draw_start_x = 0;
    int draw_end_x = sprite_width / 2.f + sprite_screen_x;

    int draw_start_y = -sprite_height / 2.f + renderer->render_height * (player->plane_height);
    draw_start_y -= (0.5 - player->height) * (renderer->render_height / transform_y);

    int draw_end_y = sprite_height / 2.f + renderer->render_height * (player->plane_height);
    if(draw_end_y >= renderer->render_height) draw_end_y = renderer->render_height - 1;

    if(draw_end_x >= renderer->render_width) draw_end_x = renderer->render_width - 1;
    for(int stripe = draw_start_x; stripe < draw_end_x; stripe++)
      {
        int tex_x = (256.f * (stripe - (-sprite_width / 2.f + sprite_screen_x)) * sprite_texture.texture.width / sprite_width) / 256.f;
        if (transform_y > 0 && stripe > 0 && stripe < renderer->render_width &&
            transform_y < z_buffer[stripe]) {

          Rectangle tex_stripe = {tex_x, 0, 1, sprite_texture.texture.height};
          Rectangle world_stripe = {stripe, draw_start_y, 1, sprite_height};
          DrawTexturePro(sprite_texture.texture, tex_stripe, world_stripe,
                         Vector2Zero(), 0, ORANGE);
        }
      }
  }
}

void draw_everything(Renderer *renderer, Player *player, Map *map) {
  BeginTextureMode(renderer->render_texture);
  ClearBackground(BLACK);
  draw_skybox(renderer, player, map);
  draw_world(renderer, player, map);
  draw_sprites(renderer, player);
  EndTextureMode();
  BeginDrawing();
  DrawTexturePro(renderer->render_texture.texture, (Rectangle){0,0,renderer->render_width, -renderer->render_height}, (Rectangle){0,0,renderer->screen_width, renderer->screen_height}, Vector2Zero(), 0, WHITE);
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
