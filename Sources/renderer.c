#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <math.h>
#include <kinc/color.h>
#include <kinc/graphics4/graphics.h>
#include <kinc/system.h>
#include <krink/color.h>
#include <krink/system.h>
#include <krink/memory.h>
#include <krink/image.h>
#include <krink/graphics2/graphics.h>
#include <krink/graphics2/stb_truetype.h>
#include <krink/graphics2/ttf.h>
#include "renderer.h"

typedef struct RenImage RenImage;
typedef struct RenFont {
  kr_ttf_font_t* data;
  float size;
  int height;
  char path[260]; 
} RenFont;
typedef enum { FONT_HINTING_NONE, FONT_HINTING_SLIGHT, FONT_HINTING_FULL } ERenFontHinting;
typedef enum { FONT_ANTIALIASING_NONE, FONT_ANTIALIASING_GRAYSCALE, FONT_ANTIALIASING_SUBPIXEL } ERenFontAntialiasing;
typedef enum { FONT_STYLE_BOLD = 1, FONT_STYLE_ITALIC = 2, FONT_STYLE_UNDERLINE = 4, FONT_STYLE_SMOOTH = 8, FONT_STYLE_STRIKETHROUGH = 16 } ERenFontStyle;
typedef struct { uint8_t b, g, r, a; } RenColor;
typedef struct { int x, y, width, height; } RenRect;



#define MAX_GLYPHSET 256

struct RenImage {
  RenColor *pixels;
  int width, height;
};

typedef struct {
  RenImage *image;
  stbtt_bakedchar glyphs[256];
} GlyphSet;

static kinc_image_t surf;
static struct { int left, top, right, bottom; } clip;

static inline uint32_t color_to_uint(RenColor color) {
	uint32_t c = 0;
	c = kr_color_set_channel(c, 'A', color.a);
	c = kr_color_set_channel(c, 'R', color.r);
	c = kr_color_set_channel(c, 'G', color.g);
	c = kr_color_set_channel(c, 'B', color.b);
	return c;
}

static void* check_alloc(void *ptr) {
  if (!ptr) {
    fprintf(stderr, "Fatal error: memory allocation failed\n");
    exit(EXIT_FAILURE);
  }
  return ptr;
}


void ren_init(void) {
  kr_g2_init();
  int w = kinc_width();
  int h = kinc_height();
}

void ren_update_rects(RenRect *rects, int count) {}

void ren_set_clip_rect(RenRect rect) {
  kr_g2_scissor(rect.x,rect.y,rect.width,rect.height);
}

void ren_pop_clip_rect(void) {
  kr_g2_disable_scissor();
}


void ren_get_size(int *x, int *y) {
  *x = kinc_width();
  *y = kinc_height();
}


// RenImage* ren_new_image(int width, int height) {
//   assert(width > 0 && height > 0);
//   RenImage *image = kr_malloc(sizeof(RenImage) + width * height * sizeof(RenColor));
//   check_alloc(image);
//   image->pixels = (void*) (image + 1);
//   image->width = width;
//   image->height = height;
//   return image;
// }


// void ren_free_image(RenImage *image) {
//   kr_free(image);
// }

struct Font{
  char fname[260];
  kr_ttf_font_t font;
};

static struct Font fonts[64];
static size_t num_fonts = 0;
// RenFont* ren_load_font(const char *filename, float size) {

//   RenFont *font = NULL;
//   FILE *fp = NULL;

//   /* init font */
//   font = check_alloc(kr_malloc(sizeof(RenFont)));
//   strcpy(font->path,filename);
//   bool found = false;
//   for(int i =0; i < num_fonts;++i){
//     if(strcmp(fonts[i].fname,filename) == 0){
//       font->data = &fonts[i].font;
//       found = true;
//     }
//   }
//   if(!found){
//     font->data = &fonts[num_fonts].font;
//     strcpy(fonts[num_fonts].fname,filename);
//     kr_ttf_font_init(font->data,filename,0);
//     num_fonts++;
//   }

//   float scale = kr_ttf_load(font->data,size);
//   font->size = size;
//   for(int i = 0; i < font->data->m_images_len;++i){
//     if((int)font->data->images[i].m_size  == (int)font->size){
//       font->height = (font->data->images[i].baseline/ scale - font->data->images[i].descent / scale + font->data->images[i].line_gap / scale) * scale + 0.5;
//     }
//   }

//   return font;

// fail:
//   if (font) { kr_free(font->data); }
//   kr_free(font);
//   return NULL;
// }


// void ren_free_font(RenFont *font) {
//   kr_free(font);
// }


// void ren_set_font_tab_width(RenFont *font, int n) {
//   for(int i = 0; i < font->data->m_images_len;++i){
//     if((int)font->data->images[i].m_size  == (int)font->size ){
//       font->data->images[i].chars['\t'].xadvance = n;
//       break;
//     }
//   }
// }


// int ren_get_font_tab_width(RenFont *font) {
//   for(int i = 0; i < font->data->m_images_len;++i){
//     if((int)font->data->images[i].m_size  == (int)font->size){
//       return font->data->images[i].chars['\t'].xadvance;
//     }
//   }
// }


// int ren_get_font_width(RenFont *font, const char *text) {
//   return kr_ttf_width(font->data,font->size,text);
// }


// int ren_get_font_height(RenFont *font) {
//   return font->height;
// }

void ren_begin_frame(void) {
  kinc_g4_begin(0);
  kr_g2_begin(0);
  kr_g2_clear(KINC_COLOR_BLACK);
  kr_g2_set_transform(kr_matrix3x3_identity());
}

void ren_end_frame(void) {
  kr_g2_end();
  kinc_g4_end(0);
}

void ren_draw_rect(RenRect rect, RenColor color) {
  kr_g2_set_color(color_to_uint(color));
  kr_g2_fill_rect(rect.x,rect.y,rect.width,rect.height);
}


void ren_draw_image(RenImage *image, RenRect *sub, int x, int y, RenColor color) {}

// int ren_draw_text(RenFont *font, const char *text, int x, int y, RenColor color) {
//   ren_set_font_tab_width(font, ren_get_font_tab_width(font));
//   kr_g2_set_color(color_to_uint(color));
//   kr_g2_set_font(font->data, font->size);
//   return kr_g2_draw_string(text,x,y);
// }

static RenColor checkcolor(lua_State *L, int idx, int def) {
  RenColor color;
  if (lua_isnoneornil(L, idx)) {
    return (RenColor) { def, def, def, 255 };
  }
  lua_rawgeti(L, idx, 1);
  lua_rawgeti(L, idx, 2);
  lua_rawgeti(L, idx, 3);
  lua_rawgeti(L, idx, 4);
  color.r = luaL_checknumber(L, -4);
  color.g = luaL_checknumber(L, -3);
  color.b = luaL_checknumber(L, -2);
  color.a = luaL_optnumber(L, -1, 255);
  lua_pop(L, 4);
  return color;
}

static int show_debug;
static int f_show_debug(lua_State *L) {
  luaL_checkany(L, 1);
  show_debug = lua_toboolean(L, 1);
  return 0;
}


static int f_get_size(lua_State *L) {
  int w, h;
  ren_get_size(&w, &h);
  lua_pushnumber(L, w);
  lua_pushnumber(L, h);
  return 2;
}


static int f_begin_frame(lua_State *L) {
  ren_begin_frame();
  return 0;
}


static int f_end_frame(lua_State *L) {
  ren_end_frame();
  return 0;
}


static int f_set_clip_rect(lua_State *L) {
  RenRect rect;
  rect.x = luaL_checknumber(L, 1);
  rect.y = luaL_checknumber(L, 2);
  rect.width = luaL_checknumber(L, 3);
  rect.height = luaL_checknumber(L, 4);
  ren_set_clip_rect(rect);
  // rencache_set_clip_rect(rect);
  return 0;
}

static void f_pop_clip_rect(lua_State* L){
  ren_pop_clip_rect();
  return 0;
}


static int f_draw_rect(lua_State *L) {
  RenRect rect;
  rect.x = luaL_checknumber(L, 1);
  rect.y = luaL_checknumber(L, 2);
  rect.width = luaL_checknumber(L, 3);
  rect.height = luaL_checknumber(L, 4);
  RenColor color = checkcolor(L, 5, 255);
  ren_draw_rect(rect,color);
  if (show_debug) {
    RenColor color = { rand(), rand(), rand(), 50 };
    ren_draw_rect(rect, color);
  }
  // rencache_draw_rect(rect, color);
  return 0;
}

static int f_draw_tri(lua_State* L){

  float x = luaL_checknumber(L, 1);
  float y = luaL_checknumber(L, 2);
  float x1 = luaL_checknumber(L, 3);
  float y1 = luaL_checknumber(L, 4);
  float x2 = luaL_checknumber(L, 5);
  float y2 = luaL_checknumber(L, 6);
  RenColor color = checkcolor(L, 7, 255);
  kr_g2_set_color(color_to_uint(color));
  kr_g2_fill_triangle(x,y,x1,y1,x2,y2);
}

// static int f_draw_text(lua_State *L) {
//   RenFont **font = luaL_checkudata(L, 1, API_TYPE_FONT);
//   const char *text = luaL_checkstring(L, 2);
//   int x = luaL_checknumber(L, 3);
//   int y = luaL_checknumber(L, 4);
//   RenColor color = checkcolor(L, 5, 255);
//   x = ren_draw_text(*font, text, x, y, color);
//   lua_pushnumber(L, x);
//   return 1;
// }


static const luaL_Reg lib[] = {
  { "show_debug",    f_show_debug    },
  { "get_size",      f_get_size      },
  { "begin_frame",   f_begin_frame   },
  { "end_frame",     f_end_frame     },
  { "set_clip_rect", f_set_clip_rect },
  { "pop_clip_rect", f_pop_clip_rect },
  { "draw_rect",     f_draw_rect     },
  { "draw_tri",      f_draw_tri      },
  // { "draw_text",     f_draw_text     },
  { NULL,            NULL            }
};

static int free_image(lua_State* L) {
  lua_getfield(L,1,"imageData");
  kr_image_t* img = lua_tolightuserdata(L,-1);

  kr_free(img);
};

static int draw_image(lua_State* L) {
  lua_getfield(L,1,"imageData");
  kr_image_t* img = lua_tolightuserdata(L,-1);
  lua_pop(L,1);

  float* pos = luaL_checkvector(L,2);
  float* scale = luaL_checkvector(L,3);
  float x = luaL_checknumber(L,4);
  float y = luaL_checknumber(L,5);
  float w = luaL_checknumber(L,6);
  float h = luaL_checknumber(L,7);

  kr_matrix3x3_t m = kr_g2_get_transform();
  kr_matrix3x3_t trans = kr_matrix3x3_translation(pos[0],pos[1]);

  m = kr_matrix3x3_multmat(&m,&trans);

  kr_g2_set_transform(m);

  int flipx = scale[2];
  int flipy = scale[3];
  kr_g2_set_color(KINC_COLOR_WHITE);
  kr_g2_draw_scaled_sub_image(img,x,y,w,h,(flipx > 0.0 ? w:0),(flipy > 0.0 ? h:0),(flipx > 0.0 ? -w:w), (flipy > 0.0 ? -h:h));
};

static int new_image(lua_State* L) {
    const char *path = luaL_checkstring(L, 1); // Get the 'path' parameter

    kr_image_t* img = kr_malloc(sizeof(kr_image_t));
    assert(img != NULL);
    kr_image_load(img,path,false);

    lua_newtable(L);
    lua_pushlightuserdata(L,img);
    lua_setfield(L, -2, "imageData");

    lua_pushcfunction(L,free_image,"free_image");
    lua_setfield(L,-2,"free_image");

    lua_pushcfunction(L,draw_image,"draw_image");
    lua_setfield(L,-2,"draw");
    
    return 1;
}

static const luaL_Reg image_methods[] = {
  { "new",      new_image      },
  { NULL,            NULL            }
};

int luaopen_renderer_font(lua_State *L);

int luaopen_renderer(lua_State *L) {
    lua_newtable(L);
    lua_setglobal(L,"Gfx");

    luaL_newmetatable(L, "Image");
    luaL_register(L, NULL, image_methods);
    lua_pushvalue(L, -1);
    lua_setfield(L, -2, "__index");
    lua_setglobal(L, "Image");

    lua_getglobal(L,"Gfx");
    luaL_register(L,NULL,lib);
    lua_pop(L,1);
    // luaopen_renderer_font(L);
    // lua_setfield(L, -2, "font");
    return 1;
}

