#include "map.h"
#include <raymath.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define READ_ARGS(type)                                                 \
    do {                                                                \
        type *args = malloc(sizeof(type));                              \
        if(args == NULL) {                                              \
            fprintf(stderr, "ERROR: Could not allocate memory for type\n"); \
            exit(1);                                                    \
        }                                                               \
        fread(args, sizeof(type), 1, fp);                               \
        data[i].args = args;                                            \
    } while (0)

Map load_map(char *path) {
    Map map = {0};
    FILE *fp = fopen(path, "rb");
    if (fp == NULL) {
        fprintf(stderr, "ERROR: Error opening file '%s'\n", path);
        exit(1);
    }
    fread(&map, sizeof(Map) - sizeof(MapTile*), 1, fp);
    if (map.size % map.width) {
        fprintf(stderr, "ERROR: Map '%s' is not rectangular\n", path);
        exit(1);
    }

    MapTile *data = malloc(map.size * sizeof(MapTile));
    if (data == NULL) {
        fprintf(stderr, "ERROR: Could not allocate memory for map data\n");
        exit(1);
    }

    unsigned long size_without_args = sizeof(MapTile) - sizeof(void*);
    for (int i = 0; i < map.size; ++i) {
        fread(&data[i], size_without_args, 1, fp);

        switch(data[i].type) {
        case TILE_TYPE_PLANE_WALL: {
            READ_ARGS(PlaneWallArgs);
            break;
        }
        default:
            data[i].args = NULL;
            break;
        }
    }
    map.data = data;
    fclose(fp);
    return map;
}

void save_map(Map *map, char *path) {
    FILE *fp = fopen(path, "wb");
    if (fp == NULL) {
        fprintf(stderr, "ERROR: Error opening file '%s'\n", path);
        exit(1);
    }
    fwrite(map, sizeof(Map) - sizeof(MapTile*), 1, fp);
    unsigned long size_without_args = sizeof(MapTile) - sizeof(void*);
    for (int i = 0; i < map->size; ++i){
        fwrite(&map->data[i], size_without_args, 1, fp);
        switch(map->data[i].type) {
        case TILE_TYPE_PLANE_WALL:
            fwrite(map->data[i].args, sizeof(PlaneWallArgs), 1, fp);
            break;
        default:
            break;
        }
    }
    fclose(fp);
}

void resize_map(Map *map, int new_width, int new_height) {
    int new_size = new_width * new_height;
    if (new_size == map->size && new_width == map->width) return;
    MapTile *new_map_data = malloc(new_size * sizeof(MapTile));
    for (int y = 0; y < new_height; ++y) {
        for (int x = 0; x < new_width; ++x) {
            Vector2 map_pos = {x,y};
            MapTile *tile;
            GET_TILE_AT_POINT_DEFAULT_EMPTY(tile, map, &map_pos);
            memcpy(&new_map_data[x + y * new_width], tile, sizeof(MapTile));
        }
    }
    free(map->data);
    map->size = new_size;
    map->width = new_width;
    map->data = new_map_data;
}

inline MapTile *get_tile_at_point(Map *map, Vector2 *position) {
    if (is_in_bounds(map, position))
        return &map->data[(int)position->x + (int)position->y * map->width];
    else
        return NULL;
}

inline void set_tile_at_point(Map *map, Vector2 position, MapTile tile) {
    if (is_in_bounds(map, &position))
        map->data[(int)position.x + (int)position.y * map->width] = tile;
}

inline bool is_in_bounds(Map *map, Vector2 *position) {
    if (position-> x < 0 || position->y < 0)
        return false;
    if (position->x >= map->width || position->y * map->width >= map->size)
        return false;
    return true;
}

Map create_empty_map(void) {
    MapTile *data = malloc(sizeof(MapTile));
    if (data == NULL) {
        fprintf(stderr, "ERROR: Could not allocate memory for new map\n");
        exit(1);
    }
    memset(data, 0, sizeof(MapTile));
    Map map = {
        .version = 1,
        .size = 1,
        .width = 1,
        .player_start_position = {0.5, 0.5},
        .skybox = 0,
        .data = data,
    };
    return map;
}
