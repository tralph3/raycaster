#ifndef MAP_H
#define MAP_H

#include "textures.h"
#include <raymath.h>


typedef enum {
  TILE_TYPE_EMPTY = 0,
  TILE_TYPE_WALL,
} __attribute__ ((__packed__)) TileType;


typedef struct {
  TextureID wall_id;
  TextureID ceiling_id;
  TextureID floor_id;
  TileType type;
} MapTile;

typedef struct {
  unsigned int version;
  unsigned int size;
  unsigned int width;
  Vector2 player_start_position;
  TextureID skybox;
  MapTile *data;
} Map;

Map load_map(char*);
void save_map(Map*, char*);
void resize_map(Map *, unsigned int, unsigned int);
MapTile get_tile_at_point(Map *, Vector2);
void set_tile_at_point(Map *, Vector2, MapTile);
bool is_in_bounds(Map *, Vector2);

#endif
