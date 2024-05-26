#ifndef RENDERER_H
#define RENDERER_H

#include <math.h>
#include <raylib.h>

#include "map1.h"
#include "player.h"
#include "textures.h"

#define SCREEN_WIDTH 1920
#define SCREEN_HEIGHT 1080

#define SCREEN_CENTER                                                          \
    (Vector2) { SCREEN_WIDTH / 2.f, SCREEN_HEIGHT / 2.f }
#define WALL_HEIGHT 900.f

const int tile_size = SCREEN_WIDTH / MAP_WIDTH;

void draw_world(Player *player, TextureArr *textures) {
    DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_CENTER.y,
                  ColorBrightness(GRAY, -0.4f));
    DrawRectangle(0, SCREEN_CENTER.y, SCREEN_WIDTH, SCREEN_CENTER.y, GRAY);
    for (int x = 0; x <= SCREEN_WIDTH; ++x) {
        int map_x = (int)player->position.x / tile_size;
        int map_y = (int)player->position.y / tile_size;
        float p_percentage_x =
            (player->position.x - tile_size * map_x) / tile_size;
        float p_percentage_y =
            (player->position.y - tile_size * map_y) / tile_size;
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

            world_tile = world_map[map_x + map_y * MAP_WIDTH];
            if (world_tile > 0)
                hit = true;
        }
        Texture2D *texture =
            &textures->items[(int)fmin(world_tile - 1, textures->count - 1)];

        if (side == 0)
            perpendicular_wall_dist = side_dist.x - delta_dist.x;
        else
            perpendicular_wall_dist = side_dist.y - delta_dist.y;
        float line_height = WALL_HEIGHT / perpendicular_wall_dist;

        double wall_x;
        if (side == 0)
            wall_x = player->position.y / tile_size +
                     perpendicular_wall_dist * ray_dir.y;
        else
            wall_x = player->position.x / tile_size +
                     perpendicular_wall_dist * ray_dir.x;
        wall_x -= floor(wall_x);

        int tex_x = (int)(wall_x * (double)texture->width);
        if (ray_dir.x > 0 && side == 0)
            tex_x = texture->width - tex_x - 1;
        if (ray_dir.y < 0 && side == 1)
            tex_x = texture->width - tex_x - 1;

        float draw_start = -line_height / 2.f + SCREEN_HEIGHT / 2.f;
        Rectangle texture_stripe = {tex_x, 0, 1, texture->height};
        Rectangle world_stripe = {x, draw_start, 1, line_height};
        DrawTexturePro(*texture, texture_stripe, world_stripe, (Vector2){0, 0},
                       0, side == 1 ? ColorBrightness(WHITE, -0.3f) : WHITE);
    }
}

void draw_everything(Player *player, TextureArr *textures) {
    BeginDrawing();
    draw_world(player, textures);
    EndDrawing();
}

#endif
