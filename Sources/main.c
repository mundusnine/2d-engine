
#include <kinc/global.h>
#include <kinc/log.h>
#include <kinc/system.h>
#include <kinc/display.h>

#include <krink/system.h>
#include <krink/graphics2/graphics.h>

#define LUAUC_PLUGIN_IMPL
#include "luauc_plugin_api.h"

#include "system.h"

typedef struct update_func {
    void* func;
    int is_lua;
} update_func_t;

static update_func_t update = {.func=NULL,.is_lua=-1};
static lua_State* L;

static size_t mem_size;
static void* memblck;

int engine_init(lua_State* L){
    
    if(update.func == NULL){
        char* err = "Call Engine.set_update before calling init. Will not initialize.";
        kinc_log(KINC_LOG_LEVEL_ERROR,err);
        luaL_error(L,err);
        return -1;
    }
    kinc_display_init();
    kinc_display_mode_t dm = kinc_display_current_mode(kinc_primary_display());

    kinc_init("luauc-engine",dm.width * 0.8,dm.height * 0.8,NULL,NULL);

    mem_size = 1024 * 1024 * 1024;// 1 Gig
    memblck = malloc(mem_size);
    kr_init(memblck,mem_size,NULL,0);
    kr_g2_init();
    kr_evt_init();
    kr_evt_add_observer(event_handler);

    return 0;
}

int engine_set_update(lua_State* L){
    
    if(update.func != NULL && update.is_lua != -1){
        if(update.is_lua){
            lua_unref(L, (int)update.func);
            update.func = NULL;
            update.is_lua =-1;
        }
    }

    if(lua_iscfunction(L,1)){
        update.func = lua_tocfunction(L,1);
        update.is_lua = 0;
    }
    else if(lua_isfunction(L,1)){
        update.func = lua_ref(L,1);
        update.is_lua = 1;
    }
    else {
        kinc_log(KINC_LOG_LEVEL_ERROR,"Parameter passed wasn't a function.");
    }
}

static const luaL_Reg* lib[] = {
    { "init" , engine_init },
    { "set_update" , engine_set_update },
    { NULL, NULL }
};

void shutdown(void* udata){
    
    kr_g2_destroy();
    free(memblck);
}

KINC_FUNC int luaopen_luauc_engine(lua_State* L, void* LUAUC) {
    luauc_plugin_init(LUAUC);

    //Needed elements
    lua_newtable(L);
    lua_setglobal(L,"Engine");

    lua_getglobal(L,"Engine");
    luaL_setfuncs(L, lib, 0);

    lua_pushlightuserdata(L,kinc_start);
    lua_setfield(L,-1,"start");

    lua_pop(L, 1);// Pop Engine

    //Superfluous elements
    luaopen_system(L);

    kinc_set_shutdown_callback(shutdown,NULL);

    return 1;
}