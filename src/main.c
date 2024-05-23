#include "map1.h"
#include "player.h"
#include "vector.h"
#include <raylib.h>
#include <raymath.h>
#include <stdio.h>

#define SCREEN_WIDTH 1920
#define SCREEN_HEIGHT 1080

#define MARGIN 1
#define LINE_LENGTH 20

const int tile_size = SCREEN_WIDTH / MAP_WIDTH;

void draw_world(Player *player) {
    float wall_height = 30.0f;
    DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT / 2, GRAY); // ceiling
    DrawRectangle(0, SCREEN_HEIGHT / 2, SCREEN_WIDTH, SCREEN_HEIGHT / 2, ColorBrightness(GRAY, -0.4f)); // floor
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
        float perpWallDist;
        Color wall_color = {0};

        double camera_x = 2 * x / (double)SCREEN_WIDTH - 1;
        Vector2 ray_dir = vec2_add(player->direction,
                                   vec2_mul(player->cameraPlane, camera_x));

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
                map_x += step_x;
                side = 0;
            } else {
                side_dist.y += delta_dist.y;
                map_y += step_y;
                side = 1;
            }

            int world_tile = world_map[map_x + map_y * MAP_WIDTH];
            if (world_tile > 0){
                hit = true;
                switch (world_tile) {
                case 1:  wall_color = RED;  break; //red
                case 2:  wall_color = GREEN;  break; //green
                case 3:  wall_color = BLUE;   break; //blue
                case 4:  wall_color = WHITE;  break; //white
                default: wall_color = YELLOW; break; //yellow
                }
            }
        }

        if (side == 0)
            perpWallDist = side_dist.x - delta_dist.x;
        else
            perpWallDist = (side_dist.y - delta_dist.y);
        float line_height = wall_height / perpWallDist;
        Vector2 line_start = {x, SCREEN_HEIGHT / 2.f + line_height / 2.f};
        Vector2 line_end = {x, SCREEN_HEIGHT / 2.f - line_height / 2.f};
        wall_color = side == 0 ? wall_color : ColorBrightness(wall_color, -0.1f);
        DrawLineEx(line_start, line_end, 1, wall_color);
    }
}

void draw_everything(Player *player) {
    BeginDrawing();
    ClearBackground(BLACK);
    draw_world(player);
    EndDrawing();
}

void handle_input(Player *player) {
    PollInputEvents();
    float multiplier = 1;
    if (IsKeyPressed(KEY_ESCAPE))
        CloseWindow();
    if (IsKeyDown(KEY_LEFT_SHIFT))
        multiplier = 3.f;
    if (IsKeyDown(KEY_D)) {
        player_rotate(player, 0.1);
    }
    if (IsKeyDown(KEY_A)) {
        player_rotate(player, -0.1);
    }
    if (IsKeyDown(KEY_W)) {
        player_move_forward(player, 0.1 * multiplier);
    }
    if (IsKeyDown(KEY_S)) {
        player_move_backward(player, 0.1 * multiplier);
    }
}

void check_collission(Player *player) {
    int map_x = player->position.x / tile_size;
    int map_y = player->position.y / tile_size;
    int tile = world_map[map_x + map_y * MAP_WIDTH];
    if (tile > 0)
        player->position = player->oldPosition;
}

int main(void) {
    Player player = {
        .position = {100, 100},
        .oldPosition = {100, 100},
        .direction = {LINE_LENGTH, 0},
        .cameraPlane = {0, 30},
    };

    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Raycaster");
    SetTargetFPS(60);
    while (!WindowShouldClose()) {
        handle_input(&player);
        check_collission(&player);
        draw_everything(&player);
    }
    CloseWindow();
    return 0;
}
