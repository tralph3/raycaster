#ifndef MAP_H
#define MAP_H

#include "textures.h"
#include <raymath.h>

#define GET_TILE_AT_POINT_DEFAULT_EMPTY(dest, map, vector)  \
    do {                                                    \
        dest = get_tile_at_point(map, vector);              \
        if (dest == NULL) dest = &(MapTile){0};             \
    } while(0)

#define GET_TILE_AT_POINT_DEFAULT_WALL(dest, map, vector)               \
    do {                                                                \
        dest = get_tile_at_point(map, vector);                          \
        if (dest == NULL) dest = &(MapTile){0, 0, 0, TILE_TYPE_WALL, 0}; \
    } while(0)

typedef enum {
    TILE_TYPE_EMPTY = 0,
    TILE_TYPE_WALL,
    TILE_TYPE_PLANE_WALL,
    TILE_TYPE_VOXEL_WALL,
} TileType;

typedef struct {
    Vector2 start;
    Vector2 end;
} PlaneWallArgs;

typedef struct {
    TextureID wall_id;
    TextureID ceiling_id;
    TextureID floor_id;
    TileType type;
    void *args;
} MapTile;

typedef struct {
    unsigned int version;
    int size;
    int width;
    Vector2 player_start_position;
    TextureID skybox;
    MapTile *data;
} Map;

Map load_map(char*);
void save_map(Map*, char*);
void resize_map(Map *, int, int);
MapTile *get_tile_at_point(Map *, Vector2 *);
void set_tile_at_point(Map *, Vector2, MapTile);
bool is_in_bounds(Map *, Vector2 *);
Map create_empty_map(void);

#endif
