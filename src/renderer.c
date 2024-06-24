#include "renderer.h"
#include "map.h"
#include <raylib.h>
#include <raymath.h>

#include <stdio.h>

void print_vector(Vector2 v) {
    printf("X: %f | Y: %f\n", v.x, v.y);
}


void draw_world(Player *player, TextureArr *textures, SpriteArr *sprites, Map *map) {
    DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_CENTER.y,
                  ColorBrightness(GRAY, -0.4f));
    DrawRectangle(0, SCREEN_CENTER.y, SCREEN_WIDTH, SCREEN_CENTER.y, GRAY);
    float stripe_perp_distance[SCREEN_WIDTH] = {0};

    for (int x = 0; x <= SCREEN_WIDTH; ++x) {
        int map_x = player->position.x;
        int map_y = player->position.y;
        float p_percentage_x = player->position.x - map_x;
        float p_percentage_y = player->position.y - map_y;
        int step_x;
        int step_y;
        bool hit = false;
        int side;
        float perpendicular_wall_dist;

        double camera_x = 2 * x / (double)SCREEN_WIDTH - 1;
        Vector2 ray_dir = Vector2Add(
            player->direction, Vector2Scale(player->cameraPlane, camera_x));

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
                map_x += step_x;
                side = 0;
            } else {
                side_dist.y += delta_dist.y;
                map_y += step_y;
                side = 1;
            }

            world_tile = get_wall_at_point(map, (Vector2){map_x, map_y});
            if (world_tile > 0)
                hit = true;
        }
        Texture2D *texture =
            &textures->items[(int)fmin(world_tile - 1, textures->count - 1)];

        if (side == 0)
            perpendicular_wall_dist = side_dist.x - delta_dist.x;
        else
            perpendicular_wall_dist = side_dist.y - delta_dist.y;
        stripe_perp_distance[x] = perpendicular_wall_dist;
        float line_height = WALL_HEIGHT / perpendicular_wall_dist;

        double wall_x;
        if (side == 0)
            wall_x = player->position.y + perpendicular_wall_dist * ray_dir.y;
        else
            wall_x = player->position.x + perpendicular_wall_dist * ray_dir.x;
        wall_x -= floor(wall_x);

        int tex_x = (int)(wall_x * (double)texture->width);
        if (ray_dir.x < 0 && side == 0)
            tex_x = texture->width - tex_x - 1;
        if (ray_dir.y > 0 && side == 1)
            tex_x = texture->width - tex_x - 1;

        float draw_start = -line_height / 2.f + SCREEN_HEIGHT / 2.f;
        Rectangle texture_stripe = {tex_x, 0, 1, texture->height};
        Rectangle world_stripe = {x, draw_start, 1, line_height};
        DrawTexturePro(*texture, texture_stripe, world_stripe, (Vector2){0, 0},
                       0, side == 1 ? ColorBrightness(WHITE, -0.3f) : WHITE);
    }

    sort_sprites(player, sprites);
    for(int i = 0; i < sprites->count; ++i) {
      Vector2 sprite_pos =
        Vector2Subtract(sprites->items[i].position, player->position);
      Texture2D sprite_texture = textures->items[sprites->items[i].texture_id];

      double invDet = 1.0 / (player->cameraPlane.x * player->direction.y - player->direction.x * player->cameraPlane.y);

      double transformX = invDet * (player->direction.y * sprite_pos.x - player->direction.x * sprite_pos.y);
      double transformY = invDet * (-player->cameraPlane.y * sprite_pos.x + player->cameraPlane.x * sprite_pos.y);

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
          DrawTexturePro(sprite_texture, tex_stripe, world_stripe,
                         (Vector2){0, 0}, 0, WHITE);
        }
      }
    }
}


void draw_everything(Player *player, TextureArr *textures, SpriteArr *sprites, Map *map) {
    BeginDrawing();
    draw_world(player, textures, sprites, map);
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
