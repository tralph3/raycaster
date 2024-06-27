#include "renderer.h"
#include "map.h"
#include "textures.h"
#include <raylib.h>
#include <raymath.h>
#include <rlgl.h>
#include "game.h"

#include <stdio.h>

const Vector2 SCREEN_CENTER = { SCREEN_WIDTH * 0.5, SCREEN_HEIGHT * 0.5 };

#define VISIBLE_TILE_MAX 1000
Vector2 visible_tiles[VISIBLE_TILE_MAX];
int visible_tile_index = 0;

void print_vector(Vector2 v) {
    printf("X: %f | Y: %f\n", v.x, v.y);
}

Vector2 cast_ray(int ray, Player *player, Map *map) {
  Vector2 map_pos = (Vector2){(int)player->position.x, (int)player->position.y};
  float p_percentage_x = player->position.x - map_pos.x;
  float p_percentage_y = player->position.y - map_pos.y;
  int step_x;
  int step_y;
  bool hit = false;


  double camera_x = (ray << 1) / (double)SCREEN_WIDTH - 1;
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
    if (world_tile.type == TILE_TYPE_WALL)
      hit = true;
  }
  return map_pos;
}
void calculate_visible_walls(Game *game) {
  Player *player = &game->player;
  Map *map = &game->map;
  bool stop_casting = false;
  int ray1 = 0;
  while (!stop_casting) {
    if (visible_tile_index >= VISIBLE_TILE_MAX) {
      stop_casting = true;
      continue;
    }
      bool different_tiles = true;
      int ray2 = SCREEN_WIDTH;
      Vector2 tile1 = cast_ray(ray1, player, map);
      while (different_tiles) {
        if (visible_tiles[visible_tile_index - 1].x == tile1.x &&
            visible_tiles[visible_tile_index - 1].y == tile1.y &&
            visible_tile_index > 0) {
          ++ray1;
          tile1 = cast_ray(ray1, player, map);
          continue;
        }
        Vector2 tile2 = cast_ray(ray2, player, map);
        if (tile1.x == tile2.x && tile1.y == tile2.y) {
          different_tiles = false;
          visible_tiles[visible_tile_index] = tile1;
          visible_tile_index++;
          if (ray2 == SCREEN_WIDTH) {
            stop_casting = true;
          } else {
            ray1 = ray2 + 1;
          }
        } else {
          ray2 = fmax(floor(ray2 / 2.f), ray1);
        }
      }
    }


}

void draw_sprites(Game *game) {
  for (int i = 0; i < game->sprites.count; ++i) {
    Sprite *sprite = &game->sprites.items[i];
    DrawBillboard(
                  game->player.camera, get_texture(&game->textures, sprite->texture_id),
                  (Vector3){sprite->position.x, 0.5, sprite->position.y}, 1, WHITE);
  }
}

void draw_floor_cell(Vector2 position, Game *game) {
  MapTile tile = get_tile_at_point(&game->map, position);
  int gl_texture_id = get_texture(&game->textures, tile.floor_id).id;
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

void draw_ceiling_cell(Vector2 position, Game *game) {
  MapTile tile = get_tile_at_point(&game->map, position);
  int gl_texture_id = get_texture(&game->textures, tile.ceiling_id).id;
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

void draw_wall_cell(Vector2 position, Game *game) {
  MapTile tile = get_tile_at_point(&game->map, position);
  int gl_texture_id = get_texture(&game->textures, tile.wall_id).id;
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

void draw_everything(Game *game) {
  visible_tile_index = 0;
  BeginDrawing();
  ClearBackground(BLACK);
  DrawTexturePro(game->sky,
                 (Rectangle){0, 0, game->sky.width, game->sky.height},
                 (Rectangle){0, 0, SCREEN_WIDTH, SCREEN_CENTER.y},
                 Vector2Zero(), 0, WHITE);

  BeginMode3D(game->player.camera);
  rlBegin(RL_QUADS);
  rlColor4ub(255, 255, 255, 255);
  calculate_visible_walls(game);
  for (unsigned int x = 0; x < game->map.width; ++x) {
    for (unsigned int y = 0; y <  game->map.size / game->map.width; ++y) {
      draw_floor_cell((Vector2){x, y}, game);
      draw_ceiling_cell((Vector2){x, y}, game);
    }
  }
  for (int i = 0; i < visible_tile_index; ++i) {
    draw_wall_cell(visible_tiles[i], game);
  }
  draw_decal((Vector2){1.34, 0}, game);
  draw_decal((Vector2){2.74, 0}, game);
  draw_decal((Vector2){2.02, 0}, game);
  draw_decal((Vector2){6.30, 0}, game);

  // 3, 4
  draw_decal((Vector2){3, 3.65}, game);
  draw_decal((Vector2){3.51, 4}, game);
  draw_decal((Vector2){4, 3.12}, game);
  draw_decal((Vector2){3.23, 3}, game);
  draw_sprites(game);
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
