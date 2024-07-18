#ifndef TEXTURES_H
#define TEXTURES_H
#include <raylib.h>

typedef struct {
  Texture2D texture;
  Color *pixels;
} TexturePixels;

typedef struct {
    int count;
    int capacity;
    TexturePixels *items;
} TextureArr;

typedef unsigned char TextureID;

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
TexturePixels load_texture(const char*);
TexturePixels get_texture(TextureArr*, TextureID);
Color get_texture_pixel(TexturePixels, int, int);

#endif
