#ifndef MAP_H
#define MAP_H

#include <raymath.h>

typedef struct {
  unsigned int version;
  unsigned int size;
  unsigned int width;
  Vector2 player_start_position;
  int *walls;
} Map;

Map load_map(char*);
void save_map(Map*, char*);
int get_wall_at_point(Map*, Vector2);

#endif
