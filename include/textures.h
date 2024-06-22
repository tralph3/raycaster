#ifndef TEXTURES_H
#define TEXTURES_H

#include <raylib.h>

typedef struct {
    int count;
    int capacity;
    Texture2D *items;
} TextureArr;

typedef int TextureID;

typedef struct {
    TextureID texture_id;
    Vector2 position;
} Sprite;

typedef struct {
    int count;
    int capacity;
    Sprite *items;
} SpriteArr;

TextureArr load_all_textures(void);

#endif