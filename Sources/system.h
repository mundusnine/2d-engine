#pragma once

#include "luauc_plugin_api.h"
#include <krink/eventhandler.h>

#ifdef __cplusplus
extern "C" {
#endif

int luaopen_system(lua_State *L);
void event_handler(kr_evt_event_t event);

#ifdef __cplusplus
}
#endif