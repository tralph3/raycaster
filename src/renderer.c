#include "renderer.h"
#include "array_queue.h"
#include "map.h"
#include "sync.h"
#include "textures.h"
#include "dynarray.h"
#include <bits/pthreadtypes.h>
#include <math.h>
#include <raylib.h>
#include <raymath.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdlib.h>
#include <stdio.h>
#include <physics.h>
#include <sys/sysinfo.h>

int render_thread_count;

ArrayQueue job_queue;

pthread_mutex_t mutex_job_queue;
pthread_mutex_t mutex_completed_jobs;

sem_t sem_job_count;
sem_t sem_frame_finished;
int completed_jobs = 0;

Player *player;
Map *map;
Renderer *renderer;

typedef enum Side {
    SIDE_RIGHT = 0,
    SIDE_LEFT,
} Side;

float *z_buffer = NULL;
pthread_t *threads = NULL;

void *thread_job(void *);

void init_renderer(Renderer *renderer) {
    INIT_MUTEX(mutex_job_queue);
    INIT_MUTEX(mutex_completed_jobs);
    INIT_SEM(sem_job_count, 0);
    INIT_SEM(sem_frame_finished, 0);

    render_thread_count = get_nprocs();
    threads = malloc(render_thread_count * sizeof(pthread_t));

    for (int i = 0; i < render_thread_count; ++i)
        pthread_create(&threads[i], NULL, thread_job, NULL);

    int current_monitor = GetCurrentMonitor();
    renderer->screen_width = GetMonitorWidth(current_monitor);
    renderer->screen_height = GetMonitorHeight(current_monitor);

    set_render_resolution(renderer, GetMonitorWidth(current_monitor), GetMonitorHeight(current_monitor));

    job_queue = *array_queue_create(renderer->render_width);

    SetTargetFPS(60);
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

inline void draw_pixel(Renderer *renderer, int x, int y, Color color) {
    renderer->screen_buffer[y + x * renderer->render_height] = color;
}

void draw_stripe(int x) {
    Vector2 map_pos = (Vector2){(int)player->position.x, (int)player->position.y};
    float p_percentage_x = player->position.x - map_pos.x;
    float p_percentage_y = player->position.y - map_pos.y;

    float camera_x = (x << 1) / (float)renderer->render_width - 1;
    Vector2 ray_dir = Vector2Add(player->direction, Vector2Scale(player->camera_plane, camera_x));

    Vector2 delta_dist = {
        .x = ray_dir.x == 0 ? 1e30 : fabs(1 / ray_dir.x),
        .y = ray_dir.y == 0 ? 1e30 : fabs(1 / ray_dir.y),
    };

    Vector2 side_dist;
    int step_x;
    int step_y;

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

    MapTile *current_tile = get_tile_at_point(map, player->position);

    int y = renderer->render_height;

    float screen_center = renderer->render_height >> 2;
    int current_ray_height = current_tile->height;

    int side;

    while (y > screen_center) {
        if (side_dist.x < side_dist.y) {
            side_dist.x += delta_dist.x;
            map_pos.x += step_x;
            side = SIDE_RIGHT;
        } else {
            side_dist.y += delta_dist.y;
            map_pos.y += step_y;
            side = SIDE_LEFT;
        }

        MapTile *tile = get_tile_at_point(map, map_pos);

        if (tile == NULL)
            break;

        Vector2 wall_stripe_position;
        float wall_x;
        float perpendicular_wall_dist;

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

        float relative_ray_to_eye_distance = current_tile->height + player->height - current_ray_height;
        float relative_ray_to_eye_distance_pixels = relative_ray_to_eye_distance / player->plane_size * renderer->render_height;
        float ray_length = relative_ray_to_eye_distance_pixels / (y - screen_center);

        float tile_to_eye_distance = current_tile->height + player->height - tile->height;
        float tile_to_eye_distance_pixels = tile_to_eye_distance / player->plane_size * renderer->render_height;

        float tile_height_pixels = tile->height / player->plane_size * renderer->render_height;

        int wall_top_y = tile_to_eye_distance_pixels / ray_length + screen_center;
        int wall_bot_y =

        current_ray_height = tile->height;

        wall_x -= (int)wall_x;
        TexturePixels wall_texture = get_texture(&renderer->textures, tile->wall_id);
        int wall_tex_x = wall_x * wall_texture.texture.width;

        if (((ray_dir.x < 0 && side == SIDE_RIGHT) || (ray_dir.y > 0 && side == SIDE_LEFT)))
            wall_tex_x = wall_texture.texture.width - wall_tex_x - 1;



        for (; y > wall_top_y && y > 0; --y) {
            int wall_tex_y = wall_texture.texture.height * ((y - initial_y) / (wall_top_y - initial_y));

            Color color = get_texture_pixel(wall_texture, wall_tex_x, wall_tex_y);
            draw_pixel(renderer, x, y, color);
        }

        Vector2 tile_coords = Vector2Add(player->position, Vector2Scale(ray_dir, ray_length));

        for (; y > screen_center; --y) {
            float ray_length = tile_to_eye_distance_pixels / (y - screen_center);

            Vector2 texture_pos =
                Vector2Add(player->position, Vector2Scale(ray_dir, ray_length));

            if (texture_pos.x != tile_coords.x || texture_pos.y != tile_coords.y)
                break;

            MapTile tile = get_tile_at_point_default_wall(map, texture_pos);

            TexturePixels floor_texture = get_texture(&renderer->textures, tile.floor_id);
            texture_pos.x -= (int)texture_pos.x;
            texture_pos.y -= (int)texture_pos.y;

            int floor_tex_x = texture_pos.x * (floor_texture.texture.width - 1);
            int floor_tex_y = texture_pos.y * (floor_texture.texture.height - 1);

            Color color = get_texture_pixel(floor_texture, floor_tex_x, floor_tex_y);
            draw_pixel(renderer, x, y, color);
        }
    }

    for (; y >= 0; --y)
        draw_pixel(renderer, x, y, BLACK);

    /* z_buffer[x] = perpendicular_wall_dist; */
    /* float line_height = renderer->render_height / perpendicular_wall_dist; */
    /* float wall_top = renderer->render_height * player->plane_center - line_height / 2.f; */
    /* wall_top -= (0.5 - player->height) * line_height; */
    /* float wall_bot = wall_top + line_height; */

    /* wall_x -= (int)wall_x; */
    /* TexturePixels wall_texture = get_texture(&renderer->textures, wall_tile.wall_id); */
    /* int tex_x = wall_x * wall_texture.texture.width; */
    /* if (((ray_dir.x < 0 && side == SIDE_RIGHT) || (ray_dir.y > 0 && side == SIDE_LEFT)) && wall_tile.type != TILE_TYPE_PLANE_WALL) */
    /*     tex_x = wall_texture.texture.width - tex_x - 1; */

    /* float center = renderer->render_height * player->plane_center; */
    /* float height = renderer->render_height * (1 - player->height); */
    /* for (int y = 0; y < wall_top; ++y) { */
    /*     float ray_length = height / (center - y); */

    /*     Vector2 texture_pos = */
    /*         Vector2Add(player->position, Vector2Scale(ray_dir, ray_length)); */

    /*     MapTile tile = get_tile_at_point_default_wall(map, texture_pos); */

    /*     TexturePixels ceiling_texture = get_texture(&renderer->textures, tile.ceiling_id); */
    /*     texture_pos.x -= (int)texture_pos.x; */
    /*     texture_pos.y -= (int)texture_pos.y; */

    /*     int ceiling_tex_x = texture_pos.x * (ceiling_texture.texture.width - 1); */
    /*     int ceiling_tex_y = texture_pos.y * (ceiling_texture.texture.height - 1); */
    /*     Color color = get_texture_pixel(ceiling_texture, ceiling_tex_x, ceiling_tex_y); */
    /*     draw_pixel(renderer, x, y, color); */
    /* } */

    /* int y_start = wall_top < 0 ? 0 : wall_top; */
    /* int y_end = wall_bot > renderer->render_height ? renderer->render_height : wall_bot; */
    /* for (int y = y_start; y <= y_end; ++y) { */
    /*     float y_percentage = Clamp((y - (int)wall_top) / line_height, 0, 1.0); */
    /*     int tex_y = y_percentage * (wall_texture.texture.height - 1); */
    /*     Color color = get_texture_pixel(wall_texture, tex_x, tex_y); */
    /*     draw_pixel(renderer, x, y, color); */
    /* } */

    /* height = renderer->render_height * player->height; */
    /* for (int y = renderer->render_height; y > wall_bot; --y) { */
    /*     float ray_length = height / (y - center); */
    /*     Vector2 texture_pos = */
    /*         Vector2Add(player->position, Vector2Scale(ray_dir, ray_length)); */

    /*     MapTile tile = get_tile_at_point_default_wall(map, texture_pos); */

    /*     TexturePixels floor_texture = get_texture(&renderer->textures, tile.floor_id); */
    /*     texture_pos.x -= (int)texture_pos.x; */
    /*     texture_pos.y -= (int)texture_pos.y; */

    /*     int floor_tex_x = texture_pos.x * (floor_texture.texture.width - 1); */
    /*     int floor_tex_y = texture_pos.y * (floor_texture.texture.height - 1); */

    /*     Color color = get_texture_pixel(floor_texture, floor_tex_x, floor_tex_y); */
    /*     draw_pixel(renderer, x, y, color); */
    /* } */
}

void draw_world(Renderer *renderer) {
    for (int i = 0; i < renderer->render_width; ++i) {
        pthread_mutex_lock(&mutex_job_queue);
        array_queue_push(&job_queue, i);
        pthread_mutex_unlock(&mutex_job_queue);

        sem_post(&sem_job_count);
    }

    sem_wait(&sem_frame_finished);

    completed_jobs = 0;

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
                   (Rectangle){0, 0, renderer->render_width, renderer->render_height * 0.5},
                   Vector2Zero(), 0, WHITE);
}

/* void draw_sprites(Renderer *renderer, Player *player) { */
/*     sort_sprites(player, &renderer->sprites); */
/*     for (int i = 0; i < renderer->sprites.count; ++i) { */
/*         Sprite sprite = renderer->sprites.items[i]; */
/*         Vector2 sprite_pos = */
/*             Vector2Subtract(renderer->sprites.items[i].position, player->position); */
/*         TexturePixels sprite_texture = get_texture(&renderer->textures, sprite.texture_id); */
/*         float inverse_determinant = 1.0f / (player->camera_plane.x * player->direction.y - player->direction.x * player->camera_plane.y); */

/*         float transform_x = inverse_determinant * (player->direction.y * sprite_pos.x - player->direction.x * sprite_pos.y); */
/*         float transform_y = inverse_determinant * (-player->camera_plane.y * sprite_pos.x + player->camera_plane.x * sprite_pos.y); */

/*         int sprite_screen_x = (renderer->render_width / 2.f) * (1 + transform_x / transform_y); */

/*         int sprite_height = fabs(renderer->render_height / transform_y); */
/*         int sprite_width = fabs( renderer->render_height / transform_y); */

/*         int draw_start_x = -sprite_width / 2.f + sprite_screen_x; */
/*         if(draw_start_x < 0) draw_start_x = 0; */
/*         int draw_end_x = sprite_width / 2.f + sprite_screen_x; */

/*         int draw_start_y = -sprite_height / 2.f + renderer->render_height * (player->plane_center); */
/*         draw_start_y -= (0.5 - player->height) * (renderer->render_height / transform_y); */

/*         int draw_end_y = sprite_height / 2.f + renderer->render_height * (player->plane_center); */
/*         if(draw_end_y >= renderer->render_height) draw_end_y = renderer->render_height - 1; */

/*         if(draw_end_x >= renderer->render_width) draw_end_x = renderer->render_width - 1; */
/*         for(int stripe = draw_start_x; stripe < draw_end_x; stripe++) */
/*         { */
/*             int tex_x = (256.f * (stripe - (-sprite_width / 2.f + sprite_screen_x)) * sprite_texture.texture.width / sprite_width) / 256.f; */
/*             if (transform_y > 0 && stripe > 0 && stripe < renderer->render_width && */
/*                 transform_y < z_buffer[stripe]) { */

/*                 Rectangle tex_stripe = {tex_x, 0, 1, sprite_texture.texture.height}; */
/*                 Rectangle world_stripe = {stripe, draw_start_y, 1, sprite_height}; */
/*                 DrawTexturePro(sprite_texture.texture, tex_stripe, world_stripe, */
/*                                Vector2Zero(), 0, ORANGE); */
/*             } */
/*         } */
/*     } */
/* } */

void *thread_job(void *_ __attribute__((unused))) {
    while (true) {
        sem_wait(&sem_job_count);

        pthread_mutex_lock(&mutex_job_queue);
        int x = array_queue_pop(&job_queue);
        pthread_mutex_unlock(&mutex_job_queue);

        draw_stripe(x);

        pthread_mutex_lock(&mutex_completed_jobs);
        ++completed_jobs;

        if (completed_jobs >= renderer->render_width) {
            pthread_mutex_unlock(&mutex_completed_jobs);
            sem_post(&sem_frame_finished);
        }

        pthread_mutex_unlock(&mutex_completed_jobs);
    }

    return NULL;
}

void draw_everything(Renderer *arg_renderer, Player *arg_player, Map *arg_map) {
    renderer = arg_renderer;
    player = arg_player;
    map = arg_map;

    BeginTextureMode(renderer->render_texture);
    ClearBackground(BLACK);
    draw_skybox(renderer, player, map);
    draw_world(renderer);
    /* draw_sprites(renderer, player); */
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
