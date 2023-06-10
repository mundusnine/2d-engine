#pragma once
/**
IMPORTANT: Define in only one .c or .cpp file ENGINE_IMPL
The luauc module API is quite simple. Any shared library can be a module file, so long
as it has an entrypoint that looks like the following, where xxxxx is your gameplay dynamic library name:
#define ENGINE_IMPL
#include "engine_api.h"
int luaopen_luauc_xxxxx(void* L, void* LUAUC) {
    engine_init(LUAUC);
    ...
    return 1;
}
Two functions will be called by default for modules i.e. module_init and module_quit.
You need to define these functions in your code.
In linux, to compile this file, you'd do: 'gcc -o xxxxx.so -shared xxxxx.c'. Simple!
IMPORTANT: **This file was automatically generated. DO NOT MODIFY DIRECTLY.**
**/

#include "stdio.h"

int module_init(void);
int module_quit(void);

typedef enum kr_evt_event_type_t kr_evt_event_type_t;
typedef struct kr_evt_key_event_t kr_evt_key_event_t;
typedef struct kr_evt_key_event_press_t kr_evt_key_event_press_t;
typedef struct kr_evt_mouse_move_event_t kr_evt_mouse_move_event_t;
typedef struct kr_evt_mouse_button_event_t kr_evt_mouse_button_event_t;
typedef struct kr_evt_mouse_scroll_event_t kr_evt_mouse_scroll_event_t;
typedef struct kr_evt_finger_touch_event_t kr_evt_finger_touch_event_t;
typedef struct kr_evt_primary_event_t kr_evt_primary_event_t;
typedef struct kr_evt_window_size_change_event_t kr_evt_window_size_change_event_t;
typedef struct kr_evt_dropfiles_event_t kr_evt_dropfiles_event_t;
typedef union kr_evt_data_t kr_evt_data_t;
typedef struct kr_evt_event_t kr_evt_event_t;
typedef struct loop_t loop_t;
int  (*poll_event)(kr_evt_event_t* e);
int  (*module_set_update)( int  (*func)(void*)  , int  id);
typedef struct RenImage RenImage;
typedef struct RenColor RenColor;
typedef struct RenRect RenRect;
void  (*ren_begin_frame)(void);
void  (*ren_end_frame)(void);
void  (*ren_draw_rect)(RenRect  rect, RenColor  color);
void  (*ren_draw_tri)(float* v, RenColor  color);
void  (*ren_draw_image)(RenImage* image, RenRect* rect, float* pos, float* scale);
RenImage* (*ren_new_image)(char* path);
void  (*ren_free_image)(RenImage* image);



void engine_init(void* LUAUC);
#ifdef ENGINE_IMPL
#define IMPORT_SYMBOL(name, ret, ...) name = (name = (ret (*) (__VA_ARGS__)) symbol(#name), name == NULL ? &__fallback_##name : name)
enum kr_evt_event_type_t{
    KR_EVT_KEY_DOWN=0,
    KR_EVT_KEY_UP=1,
    KR_EVT_KEY_PRESS=2,
    KR_EVT_MOUSE_MOVE=3,
    KR_EVT_MOUSE_PRESS=4,
    KR_EVT_MOUSE_RELEASE=5,
    KR_EVT_MOUSE_SCROLL=6,
    KR_EVT_FINGER_MOVE=7,
    KR_EVT_FINGER_START=8,
    KR_EVT_FINGER_END=9,
    KR_EVT_PRIMARY_MOVE=10,
    KR_EVT_PRIMARY_START=11,
    KR_EVT_PRIMARY_END=12,
    KR_EVT_FOREGROUND=13,
    KR_EVT_BACKGROUND=14,
    KR_EVT_PAUSE=15,
    KR_EVT_RESUME=16,
    KR_EVT_SHUTDOWN=17,
    KR_EVT_WINDOW_SIZE_CHANGE=18,
    KR_EVT_DROP_FILE=19,
};
struct kr_evt_key_event_t{
    int keycode;
};
struct kr_evt_key_event_press_t{
    unsigned character;
};
struct kr_evt_mouse_move_event_t{
    int window;
    int x;
    int y;
    int dx;
    int dy;
};
struct kr_evt_mouse_button_event_t{
    int window;
    int button;
    int x;
    int y;
};
struct kr_evt_mouse_scroll_event_t{
    int window;
    int delta;
};
struct kr_evt_finger_touch_event_t{
    int finger;
    int x;
    int y;
};
struct kr_evt_primary_event_t{
    int x;
    int y;
};
struct kr_evt_window_size_change_event_t{
    int window;
    int width;
    int height;
};
struct kr_evt_dropfiles_event_t{
    char filename[260];
};
union kr_evt_data_t{
    kr_evt_key_event_t key;
    kr_evt_key_event_press_t key_press;
    kr_evt_mouse_move_event_t mouse_move;
    kr_evt_mouse_button_event_t mouse_button;
    kr_evt_mouse_scroll_event_t mouse_scroll;
    kr_evt_finger_touch_event_t touch;
    kr_evt_primary_event_t primary;
    kr_evt_window_size_change_event_t window;
    kr_evt_dropfiles_event_t drop;
};
struct kr_evt_event_t{
    kr_evt_event_type_t event;
    kr_evt_data_t data;
};
struct loop_t{
    double* dt;
    int* end_state;
};
static int  __fallback_poll_event(kr_evt_event_t* e) { fputs("warning: poll_event is a stub\n",stderr); }
static int  __fallback_module_set_update( int  (*func)(void*)  , int  id) { fputs("warning: module_set_update is a stub\n",stderr); }
struct RenImage{
    void* ren_data;
    int width;
    int height;
};
struct RenColor{
    unsigned char b;
    unsigned char g;
    unsigned char r;
    unsigned char a;
};
struct RenRect{
    int x;
    int y;
    int width;
    int height;
};
static void  __fallback_ren_begin_frame(void) { fputs("warning: ren_begin_frame is a stub\n",stderr); }
static void  __fallback_ren_end_frame(void) { fputs("warning: ren_end_frame is a stub\n",stderr); }
static void  __fallback_ren_draw_rect(RenRect  rect, RenColor  color) { fputs("warning: ren_draw_rect is a stub\n",stderr); }
static void  __fallback_ren_draw_tri(float* v, RenColor  color) { fputs("warning: ren_draw_tri is a stub\n",stderr); }
static void  __fallback_ren_draw_image(RenImage* image, RenRect* rect, float* pos, float* scale) { fputs("warning: ren_draw_image is a stub\n",stderr); }
static RenImage* __fallback_ren_new_image(char* path) { fputs("warning: ren_new_image is a stub\n",stderr); }
static void  __fallback_ren_free_image(RenImage* image) { fputs("warning: ren_free_image is a stub\n",stderr); }
void engine_init(void* LUAUC){
    void* (*symbol)(const char *) = (void* (*) (const char *)) LUAUC;
    IMPORT_SYMBOL(poll_event,int , kr_evt_event_t*);
    IMPORT_SYMBOL(module_set_update,int ,  int  (*func)(void*) , int );
    IMPORT_SYMBOL(ren_begin_frame,void , void);
    IMPORT_SYMBOL(ren_end_frame,void , void);
    IMPORT_SYMBOL(ren_draw_rect,void , RenRect , RenColor );
    IMPORT_SYMBOL(ren_draw_tri,void , float*, RenColor );
    IMPORT_SYMBOL(ren_draw_image,void , RenImage*, RenRect*, float*, float*);
    IMPORT_SYMBOL(ren_new_image,RenImage*, char*);
    IMPORT_SYMBOL(ren_free_image,void , RenImage*);
}

#endif//ENGINE_IMPL