#pragma once

#include "luauc_plugin_api.h"
#include <krink/eventhandler.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    double* dt;
    int* end_state;
} loop_t;

int luaopen_system(lua_State *L);
void luaevent_handler(kr_evt_event_t event);
int poll_event(kr_evt_event_t* e);
int module_set_update(int (*func)(void* /*data*/),int id);
int module_reload_asset(const char* filename);

#ifdef __cplusplus
}
#endif