#include "renderer.h"
#include "map.h"
#include "textures.h"
#include <raylib.h>
#include <raymath.h>
#include <rlgl.h>
#include "game.h"

#include <stdio.h>

#define VISIBLE_TILE_MAX 1000

Vector2 visible_tiles[VISIBLE_TILE_MAX];
Vector2 visible_floor_tiles[VISIBLE_TILE_MAX];
int visible_tile_index = 0;
int visible_floor_tile_index = 0;

void print_vector(Vector2 v) {
  printf("X: %f | Y: %f\n", v.x, v.y);
}

Vector2 cast_ray(Renderer *renderer, int ray, Player *player, Map *map) {
  Vector2 map_pos = (Vector2){(int)player->position.x, (int)player->position.y};
  float p_percentage_x = player->position.x - map_pos.x;
  float p_percentage_y = player->position.y - map_pos.y;
  int step_x;
  int step_y;
  bool hit = false;


  double camera_x = (ray << 1) / (double)renderer->render_width - 1;
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

  MapTile world_tile;
  while (!hit) {
    if (side_dist.x < side_dist.y) {
      side_dist.x += delta_dist.x;
      map_pos.x += step_x;
    } else {
      side_dist.y += delta_dist.y;
      map_pos.y += step_y;
    }

    world_tile = get_tile_at_point(map, map_pos);
    if (world_tile.type == TILE_TYPE_WALL) {
      hit = true;
    } else {
      bool tile_was_previously_seen = false;
      for (int i = 0; i < visible_floor_tile_index; ++i) {
        if (visible_floor_tiles[i].x == map_pos.x && visible_floor_tiles[i].y == map_pos.y) {
          tile_was_previously_seen = true;
          break;
        }
      }
      if (!tile_was_previously_seen) {
        visible_floor_tiles[visible_floor_tile_index] = map_pos;
        ++visible_floor_tile_index;
      }
    }
  }
  return map_pos;
}

void calculate_visible_walls(Renderer *renderer, Player *player, Map *map) {
  bool stop_casting = false;
  int ray1 = 0;
  visible_floor_tiles[visible_floor_tile_index] = (Vector2){(int)player->position.x, (int)player->position.y};
  ++visible_floor_tile_index;
  while (!stop_casting) {
    if (visible_tile_index >= VISIBLE_TILE_MAX) {
      stop_casting = true;
      continue;
    }
    bool different_tiles = true;
    int ray2 = renderer->render_width;
    Vector2 tile1 = cast_ray(renderer, ray1, player, map);
    while (different_tiles) {
      if (visible_tiles[visible_tile_index - 1].x == tile1.x &&
          visible_tiles[visible_tile_index - 1].y == tile1.y &&
          visible_tile_index > 0) {
        ++ray1;
        tile1 = cast_ray(renderer, ray1, player, map);
        continue;
      }
      Vector2 tile2 = cast_ray(renderer, ray2, player, map);
      if (tile1.x == tile2.x && tile1.y == tile2.y) {
        different_tiles = false;
        visible_tiles[visible_tile_index] = tile1;
        visible_tile_index++;
        if (ray2 == renderer->render_width)
          stop_casting = true;
        else
          ray1 = ray2 + 1;
      } else {
        ray2 = fmax(floor(ray2 / 2.f), ray1);
      }
    }
  }
}

void draw_sprites(Renderer *renderer, Camera *camera) {
  for (int i = 0; i < renderer->sprites.count; ++i) {
    Sprite *sprite = &renderer->sprites.items[i];
    DrawBillboard(
                  *camera, get_texture(&renderer->textures, sprite->texture_id),
                  (Vector3){sprite->position.x, 0.5, sprite->position.y}, 1, WHITE);
  }
}

void draw_floor_cell(Vector2 position, Renderer *renderer, Map *map) {
  MapTile tile = get_tile_at_point(map, position);
  int gl_texture_id = get_texture(&renderer->textures, tile.floor_id).id;
  rlSetTexture(gl_texture_id);

  rlTexCoord2f(0, 0);
  rlVertex3f(position.x, 0, position.y);

  rlTexCoord2f(0, 1);
  rlVertex3f(position.x, 0, position.y + 1);

  rlTexCoord2f(1, 1);
  rlVertex3f(position.x + 1, 0, position.y + 1);

  rlTexCoord2f(1, 0);
  rlVertex3f(position.x + 1, 0, position.y);
}

void draw_ceiling_cell(Vector2 position, Renderer *renderer, Map *map) {
  MapTile tile = get_tile_at_point(map, position);
  int gl_texture_id = get_texture(&renderer->textures, tile.ceiling_id).id;
  rlSetTexture(gl_texture_id);

  rlTexCoord2f(1, 0);
  rlVertex3f(position.x + 1, 1, position.y);

  rlTexCoord2f(1, 1);
  rlVertex3f(position.x + 1, 1, position.y + 1);

  rlTexCoord2f(0, 1);
  rlVertex3f(position.x, 1, position.y + 1);

  rlTexCoord2f(0, 0);
  rlVertex3f(position.x, 1, position.y);
}

void draw_wall_cell(Vector2 position, Renderer *renderer, Map *map) {
  MapTile tile = get_tile_at_point(map, position);
  int gl_texture_id = get_texture(&renderer->textures, tile.wall_id).id;
  rlSetTexture(gl_texture_id);

  // LEFT SIDE QUAD
  rlTexCoord2f(0, 0);
  rlVertex3f(position.x, 1, position.y);

  rlTexCoord2f(0, 1);
  rlVertex3f(position.x, 0, position.y);

  rlTexCoord2f(1, 1);
  rlVertex3f(position.x, 0, position.y + 1);

  rlTexCoord2f(1, 0);
  rlVertex3f(position.x, 1, position.y + 1);

  // FRONT SIDE QUAD
  rlTexCoord2f(0, 0);
  rlVertex3f(position.x, 1, position.y + 1);

  rlTexCoord2f(0, 1);
  rlVertex3f(position.x, 0, position.y + 1);

  rlTexCoord2f(1, 1);
  rlVertex3f(position.x + 1, 0, position.y + 1);

  rlTexCoord2f(1, 0);
  rlVertex3f(position.x + 1, 1, position.y + 1);

  // RIGHT SIDE QUAD
  rlTexCoord2f(0, 0);
  rlVertex3f(position.x + 1, 1, position.y + 1);

  rlTexCoord2f(0, 1);
  rlVertex3f(position.x + 1, 0, position.y + 1);

  rlTexCoord2f(1, 1);
  rlVertex3f(position.x + 1, 0, position.y);

  rlTexCoord2f(1, 0);
  rlVertex3f(position.x + 1, 1, position.y);


  // BACK SIDE QUAD
  rlTexCoord2f(0, 0);
  rlVertex3f(position.x + 1, 1, position.y);

  rlTexCoord2f(0, 1);
  rlVertex3f(position.x + 1, 0, position.y);

  rlTexCoord2f(1, 1);
  rlVertex3f(position.x, 0, position.y);

  rlTexCoord2f(1, 0);
  rlVertex3f(position.x, 1, position.y);
}

void draw_decal(Vector2 position, Game *game) {
  Texture2D decal = get_texture(&game->textures,11);
  rlSetTexture(decal.id);
  float scale = 0.007;
  float scaled_width = decal.width * scale;
  float scaled_height = decal.height * scale;
  rlTexCoord2f(0, 0);
  rlVertex3f(position.x, 0.5 + (scaled_height / 2), position.y + 1.0005);

  rlTexCoord2f(0, 1);
  rlVertex3f(position.x, 0.5 - (scaled_height / 2), position.y + 1.0005);

  rlTexCoord2f(1, 1);
  rlVertex3f(position.x + scaled_width, 0.5 - (scaled_height / 2), position.y + 1.0005);

  rlTexCoord2f(1, 0);
  rlVertex3f(position.x + scaled_width, 0.5 + (scaled_height / 2), position.y + 1.0005);
}

void draw_everything(Renderer *renderer, Player *player, Map *map) {
  visible_tile_index = 0;
  visible_floor_tile_index = 0;
  BeginDrawing();
  ClearBackground(BLACK);
  /* DrawTexturePro(game->sky, */
  /*                (Rectangle){0, 0, game->sky.width, game->sky.height}, */
  /*                (Rectangle){0, 0, SCREEN_WIDTH, SCREEN_CENTER.y}, */
  /*                Vector2Zero(), 0, WHITE); */

  BeginMode3D(player->camera);
  rlBegin(RL_QUADS);
  rlColor4ub(255, 255, 255, 255);
  calculate_visible_walls(renderer, player, map);
  for (int i = 0; i < visible_floor_tile_index; ++i) {
    draw_floor_cell(visible_floor_tiles[i], renderer, map);
    draw_ceiling_cell(visible_floor_tiles[i], renderer, map);
  }
  for (int i = 0; i < visible_tile_index; ++i)
    draw_wall_cell(visible_tiles[i], renderer, map);
  draw_sprites(renderer, &player->camera);
  rlEnd();
  EndMode3D();
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
