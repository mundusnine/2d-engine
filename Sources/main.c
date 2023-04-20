
#include <stdlib.h>

#include <kinc/global.h>
#include <kinc/log.h>
#include <kinc/system.h>
#include <kinc/display.h>
#include <kinc/threads/atomic.h>

#include <krink/system.h>
#include <krink/graphics2/graphics.h>

#define LUAUC_PLUGIN_IMPL
#include "luauc_plugin_api.h"

#include "system.h"
#include "renderer.h"

typedef struct update_func {
    void* func;
    int is_lua;
} update_func_t;

update_func_t server_update = {.func=NULL,.is_lua=-1};
update_func_t runtime_update = {.func=NULL,.is_lua=-1};
lua_State* local_L;

size_t mem_size;
void* memblck;

int engine_init(lua_State* L){
    
    if(server_update.func == NULL){
        char* err = "Call Engine.set_update before calling init. Will not initialize.";
        kinc_log(KINC_LOG_LEVEL_ERROR,err);
        luaL_error(L,err);
        return -1;
    }
    kinc_display_init();
    kinc_display_mode_t dm = kinc_display_current_mode(kinc_primary_display());

    kinc_init("middle-engine",dm.width * 0.8,dm.height * 0.8,NULL,NULL);

    mem_size = 1024 * 1024;// * 1024;
    memblck = malloc(mem_size);
    kr_init(memblck,mem_size,NULL,0);
    kr_g2_init();
    kr_evt_init();
    kr_evt_add_observer(event_handler);

    return 0;
}

int engine_set_update(lua_State* L){
    update_func_t* update = (lua_isnil(L,2) || !lua_toboolean(L,2)) ? &runtime_update : &server_update;
    if(update->func != NULL && update->is_lua != -1){
        if(update->is_lua){
            lua_unref(L, (int)update->func);
            update->func = NULL;
            update->is_lua =-1;
        }
    }

    if(lua_iscfunction(L,1)){
        update->func = lua_tocfunction(L,1);
        update->is_lua = 0;
    }
    else if(lua_isfunction(L,1)){
        update->func = lua_ref(L,1);
        update->is_lua = 1;
    }
    #ifndef NDEBUG
    else {
        kinc_log(KINC_LOG_LEVEL_ERROR,"Parameter passed wasn't a function.");
    }
    #endif
}

int* end_state;
void engine_quit(lua_State* L){
    if(!lua_isnoneornil(L,1) && lua_isboolean(L,1)){
        *end_state = lua_toboolean(L,1);
    }
    local_L = end_state = NULL;

    runtime_update.func = server_update.func = NULL;
    runtime_update.is_lua = server_update.is_lua = -1;

    kr_g2_destroy();
    free(memblck);
    kinc_stop();

    luauc_plugin_uninit();
}

static const luaL_Reg lib[] = {
    { "init" , engine_init },
    { "quit" , engine_quit },
    { "set_update" , engine_set_update },
    { NULL, NULL }
};

int error_handler(lua_State* L) {
    char message[256] = {0}; 
    printf("Luau engine Error: %s\n",lua_tostring(L, 1));
    return 0;
}

void game_loop(void* udata){
    lua_State* L = local_L;
    if(server_update.func != NULL){
        ((lua_CFunction)server_update.func)(L);
    }

    if(runtime_update.func == NULL) return;
    if(runtime_update.is_lua){
        lua_getref(L,runtime_update.func);
        lua_pcall(L,0,1,0);
    }
    else{
        ((lua_CFunction)runtime_update.func)(L);
    }
    lua_settop(L, 0);
}

KINC_FUNC int luaopen_luauc_engine(lua_State* L, void* LUAUC) {
    luauc_plugin_init(LUAUC);

    local_L = L;
    //Needed elements
    lua_newtable(L);
    lua_setglobal(L,"Engine");

    lua_getglobal(L,"Engine");
    luaL_register(L,NULL,lib);

    lua_pushcfunction(L,kinc_start,"kinc_start");
    lua_setfield(L,-2,"start");

    lua_pop(L, 1);// Pop Engine

    //Superfluous elements
    luaopen_system(L);
    luaopen_renderer(L);

    kinc_set_update_callback(game_loop,NULL);

    lua_getglobal(L,"is_runtime_active");
    end_state = (int*)lua_tolightuserdata(L,-1);

    return 1;
}