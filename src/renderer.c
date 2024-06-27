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

  int world_tile;
  while (!hit) {
    if (side_dist.x < side_dist.y) {
      side_dist.x += delta_dist.x;
      map_pos.x += step_x;
    } else {
      side_dist.y += delta_dist.y;
      map_pos.y += step_y;
    }

    world_tile = get_wall_at_point(map, map_pos);
    if (world_tile > 0)
      hit = true;
  }
  return map_pos;
}
void draw_world(Game *game) {
  Player *player = &game->player;
  TextureArr *textures = &game->textures;
  SpriteArr *sprites = &game->sprites;
  Map *map = &game->map;
  float stripe_perp_distance[SCREEN_WIDTH] = {0};
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

    sort_sprites(player, sprites);
    for(int i = 0; i < sprites->count; ++i) {
      Vector2 sprite_pos =
        Vector2Subtract(sprites->items[i].position, player->position);
      ColorArray sprite_texture = textures->items[sprites->items[i].texture_id];

      double invDet = 1.0 / (player->camera_plane.x * player->direction.y - player->direction.x * player->camera_plane.y);

      double transformX = invDet * (player->direction.y * sprite_pos.x - player->direction.x * sprite_pos.y);
      double transformY = invDet * (-player->camera_plane.y * sprite_pos.x + player->camera_plane.x * sprite_pos.y);

      int spriteScreenX = (SCREEN_WIDTH / 2.f) * (1 + transformX / transformY);

      int spriteHeight = fabs(SCREEN_HEIGHT / (transformY));
      int drawStartY = -spriteHeight / 2 + SCREEN_HEIGHT / 2;
      int drawEndY = spriteHeight / 2 + SCREEN_HEIGHT / 2;
      if(drawEndY >= SCREEN_HEIGHT) drawEndY = SCREEN_HEIGHT - 1;

      int spriteWidth = fabs( (SCREEN_HEIGHT / (transformY)));
      int drawStartX = -spriteWidth / 2 + spriteScreenX;
      if(drawStartX < 0) drawStartX = 0;
      int drawEndX = spriteWidth / 2 + spriteScreenX;
      if(drawEndX >= SCREEN_WIDTH) drawEndX = SCREEN_WIDTH - 1;
      for(int stripe = drawStartX; stripe < drawEndX; stripe++)
      {
        int texX = (256 * (stripe - (-spriteWidth / 2 + spriteScreenX)) * sprite_texture.width / spriteWidth) / 256;
        if (transformY > 0 && stripe > 0 && stripe < SCREEN_WIDTH &&
            transformY < stripe_perp_distance[stripe]) {

          Rectangle tex_stripe = {texX, 0, 1, sprite_texture.height};
          Rectangle world_stripe = {stripe, drawStartY, 1, spriteHeight};
          DrawTexturePro(sprite_texture.texture, tex_stripe, world_stripe, Vector2Zero(), 0, WHITE);
        }
      }
    }
}

void draw_floor_cell(Vector2 position, Game *game) {
  int tile = get_floor_at_point(&game->map, position) - 1;
  if(tile == - 1) return;
  int gl_texture_id = game->textures.items[tile].texture.id;
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
  int tile = get_ceiling_at_point(&game->map, position) - 1;
  if(tile == - 1) return;
  int gl_texture_id = game->textures.items[tile].texture.id;
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
  int tile = get_wall_at_point(&game->map, position) - 1;
  int gl_texture_id = game->textures.items[tile].texture.id;
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

void draw_everything(Game *game) {
  visible_tile_index = 0;
  BeginDrawing();
  ClearBackground(BLACK);
  DrawTexturePro(game->sky,
                 (Rectangle){0, 0, game->sky.width, game->sky.height},
                 (Rectangle){0, 0, SCREEN_WIDTH, SCREEN_CENTER.y},
                 Vector2Zero(), 0, WHITE);
    draw_world(game);

  BeginMode3D(game->player.camera);
  rlBegin(RL_QUADS);
  rlColor4ub(255, 255, 255, 255);

  for (int x = 0; x < 24; ++x) {
    for (int y = 0; y < 24; ++y) {
      draw_floor_cell((Vector2){x, y}, game);
      draw_ceiling_cell((Vector2){x, y}, game);
      }
  }
  for (int i = 0; i < visible_tile_index; ++i) {
    draw_wall_cell(visible_tiles[i], game);
  }
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
