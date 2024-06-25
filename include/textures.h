#ifndef TEXTURES_H
#define TEXTURES_H

#include <raylib.h>

typedef struct {
    int width;
    int height;
    Color *data;
    Texture2D texture;
} ColorArray;

typedef struct {
    int count;
    int capacity;
    ColorArray *items;
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

ColorArray load_texture(char*);
TextureArr load_all_textures(void);
Color get_pixel_color(ColorArray*, Vector2);

#endif
