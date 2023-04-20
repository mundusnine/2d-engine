#pragma once

#include "luauc_plugin_api.h"

#ifdef __cplusplus
extern "C" {
#endif

#define API_TYPE_FONT "Font"
int luaopen_renderer(lua_State *L);

#ifdef __cplusplus
}
#endif