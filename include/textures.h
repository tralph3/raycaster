#ifndef TEXTURES_H
#define TEXTURES_H

#include <raylib.h>

typedef struct TexturePixels {
    Texture2D texture;
    Color *pixels;
} TexturePixels;

typedef struct TextureArr {
    int count;
    int capacity;
    TexturePixels *items;
} TextureArr;

typedef unsigned short TextureID;

typedef struct Sprite {
    TextureID texture_id;
    Vector2 position;
} Sprite;

typedef struct SpriteArr {
    int count;
    int capacity;
    Sprite *items;
} SpriteArr;

TextureArr load_all_textures(void);
TexturePixels load_texture(const char*);
TexturePixels get_texture(TextureArr*, TextureID);
Color get_texture_pixel(TexturePixels, int, int);

#endif
