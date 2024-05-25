#include "map1.h"
#include "player.h"
#include "vector.h"
#include <math.h>
#include <raylib.h>
#include <raymath.h>
#include <stdio.h>

#define SCREEN_WIDTH 1920
#define SCREEN_HEIGHT 1080

#define MARGIN 1
#define LINE_LENGTH 20

const int tile_size = SCREEN_WIDTH / MAP_WIDTH;

typedef struct {
    Color *pixels;
    int width;
    int height;
} Tex;

void draw_world(Player *player, Texture2D *textures) {
    float wall_height = 30.0f;
    DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT / 2,
                  ColorBrightness(GRAY, -0.4f));
    DrawRectangle(0, SCREEN_HEIGHT / 2, SCREEN_WIDTH, SCREEN_HEIGHT / 2, GRAY);
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

        if (side == 0)
            perpendicular_wall_dist = side_dist.x - delta_dist.x;
        else
            perpendicular_wall_dist = side_dist.y - delta_dist.y;
        float line_height = wall_height / perpendicular_wall_dist;

        // Calculate value of wall_x
        double wall_x;
        if (side == 0)
            wall_x = player->position.y / tile_size +
                     perpendicular_wall_dist * ray_dir.y;
        else
            wall_x = player->position.x / tile_size +
                     perpendicular_wall_dist * ray_dir.x;
        wall_x -= floor(wall_x);

        int tex_x = (int)(wall_x * (double)64);
        if (side == 0 && ray_dir.x > 0)
            tex_x = 64 - tex_x - 1;
        if (side == 1 && ray_dir.y < 0)
            tex_x = 64 - tex_x - 1;

        int draw_start = -line_height / 2.f + SCREEN_HEIGHT / 2.f;
        Rectangle texture_stripe = {tex_x, 0, 1, 64};
        Rectangle world_stripe = {x, draw_start, 1, line_height};
        DrawTexturePro(textures[(int)fmin(world_tile - 1, 1)], texture_stripe,
                       world_stripe, (Vector2){0, 0}, 0, WHITE);
    }
}

void draw_everything(Player *player, Texture *textures) {
    BeginDrawing();
    draw_world(player, textures);
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

    Texture2D textures[2] = {
        LoadTexture("./textures/bluestone.png"),
        LoadTexture("./textures/eagle.png"),
    };
    while (!WindowShouldClose()) {
        handle_input(&player);
        check_collission(&player);
        draw_everything(&player, textures);
        printf("%d\n", GetFPS());
    }
    CloseWindow();
    return 0;
}
