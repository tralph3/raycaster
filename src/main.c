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

void draw_world(Player *player, Color **textures, Image *buffer) {
    float wall_height = 30.0f;
    ImageDrawRectangle(buffer, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT / 2, ColorBrightness(GRAY, -0.4f));
    ImageDrawRectangle(buffer, 0, SCREEN_HEIGHT / 2, SCREEN_WIDTH, SCREEN_HEIGHT / 2,
                  GRAY);
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
            if (world_tile > 0) {
                hit = true;
                switch (world_tile) {
                case 1:
                    wall_color = RED;
                    break;
                case 2:
                    wall_color = GREEN;
                    break;
                case 3:
                    wall_color = BLUE;
                    break;
                case 4:
                    wall_color = WHITE;
                    break;
                default:
                    wall_color = YELLOW;
                    break;
                }
            }
        }

        if (side == 0)
            perpendicular_wall_dist = side_dist.x - delta_dist.x;
        else
            perpendicular_wall_dist = side_dist.y - delta_dist.y;
        float line_height = wall_height / perpendicular_wall_dist;
        int draw_start = -line_height / 2 + SCREEN_HEIGHT / 2;
        if (draw_start < 0) draw_start = 0;
        int draw_end = line_height / 2 + SCREEN_HEIGHT / 2;
        if (draw_end >= SCREEN_HEIGHT) draw_end = SCREEN_HEIGHT - 1;

        // Calculate value of wall_x
        double wall_x;
        if (side == 0) wall_x = player->position.y / tile_size + perpendicular_wall_dist * ray_dir.y;
        else wall_x = player->position.x / tile_size + perpendicular_wall_dist * ray_dir.x;
        wall_x -= floor(wall_x);

        // x coordinate on the texture
        int tex_x = (int)(wall_x * (double)64);
        if (side == 0 && ray_dir.x > 0) tex_x = 64 - tex_x - 1;
        if (side == 1 && ray_dir.y < 0) tex_x = 64 - tex_x - 1;

        // How much to increase the texture coordinate per screen pixel
        double step = 1.0 * 64 / line_height;
        // Starting texture coordinate
        double tex_pos = (draw_start - SCREEN_HEIGHT / 2 + line_height / 2) * step;

        Color tex_color;
        for (int y = draw_start; y < draw_end; y++) {
            // Cast the texture coordinate to integer, and mask with (texHeight - 1) in case of overflow
            int tex_y = (int)tex_pos & (64 - 1);
            tex_pos += step;
            // Get the color from the texture
            if (world_tile == 1)
                tex_color = textures[0][tex_x + tex_y * 64];
            else if (world_tile > 1)
                tex_color = textures[1][tex_x + tex_y * 64];
            // Draw the pixel
            if (side == 0) tex_color = ColorBrightness(tex_color, -0.2f);
            ImageDrawPixel(buffer, x, y, tex_color);
        }
    }
}

void image_to_color_array(Image *image, Color *arr) {
    if (image->format != PIXELFORMAT_UNCOMPRESSED_R8G8B8) {
        printf("Error: Unknown image format: %d\n", image->format);
    }

    for (int i = 0; i < image->width * image->height * 3; i+=3) {
        char r, g, b;
        r = ((char *)image->data)[i];
        g = ((char *)image->data)[i + 1];
        b = ((char *)image->data)[i + 2];
        Color pixel_color = {r, g, b, 255};
        arr[i/3] = pixel_color;
    }
}

void draw_everything(Player *player, Color** textures, Image *buffer) {
    BeginDrawing();
    draw_world(player, textures, buffer);
    Texture2D screen = LoadTextureFromImage(*buffer);
    DrawTexture(screen, 0, 0, WHITE);
    EndDrawing();
    UnloadTexture(screen);
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

    Color *textures[2];
    Image bluestone = LoadImage("./textures/bluestone.png");
    Color bluestone_arr[bluestone.width * bluestone.height];
    image_to_color_array(&bluestone, bluestone_arr);
    Image eagle = LoadImage("./textures/eagle.png");
    Color eagle_arr[eagle.width * eagle.height];
    image_to_color_array(&eagle, eagle_arr);
    textures[0] = bluestone_arr;
    textures[1] = eagle_arr;



    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Raycaster");
    SetTargetFPS(60);
    BeginDrawing();
    Image buffer = LoadImageFromScreen();
    EndDrawing();
    while (!WindowShouldClose()) {
        handle_input(&player);
        check_collission(&player);
        draw_everything(&player, textures, &buffer);
    }
    CloseWindow();
    return 0;
}
