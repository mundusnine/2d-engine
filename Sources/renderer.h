#pragma once

#include "luauc_plugin_api.h"

#ifdef __cplusplus
extern "C" {
#endif

#define API_TYPE_FONT "Font"
int luaopen_renderer(lua_State *L);

typedef struct { void* ren_data; int width; int height;} RenImage;
typedef struct  { unsigned char b, g, r, a; } RenColor;
typedef struct { int x, y, width, height; } RenRect;

void ren_begin_frame(void);
void ren_end_frame(void);

void ren_draw_rect(RenRect rect, RenColor color);
void ren_draw_tri(float* v, RenColor color);
void ren_draw_image(RenImage* image, RenRect* rect, float* pos, float* scale);

RenImage* ren_new_image(const char* path);
void ren_free_image(RenImage *image);

#ifdef __cplusplus
}
#endif