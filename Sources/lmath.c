#include "lmath.h"

#include "string.h"

static int f_vector_new(lua_State* L)
{
    int n = lua_gettop(L);
    double x = luaL_checknumber(L, 1);
    double y = luaL_checknumber(L, 2);
    double z = luaL_checknumber(L, 3);
    double w = n >3 ? luaL_optnumber(L, 4, 0.0) : 0.0;
    lua_pushvector(L, (float)x, (float)y, (float)z, (float)w);
    return 1;
}

static int f_vector_dot(lua_State* L)
{
    const float* a = luaL_checkvector(L, 1);
    const float* b = luaL_checkvector(L, 2);

    lua_pushnumber(L, a[0] * b[0] + a[1] * b[1] + a[2] * b[2]);
    return 1;
}

static int f_vector_add(lua_State* L)
{
    const float* a = (float*)luaL_checkvector(L, 1);
    const float x = lua_isnumber(L,2) ? lua_tonumber(L,2) : 0.0f;
    const float y = lua_isnumber(L,3) ? lua_tonumber(L,3) : 0.0f;
    const float z = lua_isnumber(L,4) ? lua_tonumber(L,4) : 0.0f;

    lua_pushvector(L,a[0] + x,a[1] + y,a[2] + z,0);

    return 1;
}

static int f_vector_2dot(lua_State* L)
{
    const float* a = luaL_checkvector(L, 1);
    const float* b = luaL_checkvector(L, 2);

    lua_pushnumber(L, a[0] * b[0] + a[1] * b[1]);
    return 1;
}

static int f_vector_index(lua_State* L)
{
    const float* v = luaL_checkvector(L, 1);
    const char* name = luaL_checkstring(L, 2);

    if (strcmp(name, "Magnitude") == 0){
        lua_pushnumber(L, sqrtf(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]));
        return 1;
    }

    if (strcmp(name, "one") == 0){
        lua_pushvector(L, 1.0f, 1.0f, 1.0f, 1.0f);
        return 1;
    }

    if (strcmp(name, "zero") == 0){
        lua_pushvector(L, 0.0f, 0.0f, 0.0f, 0.0f);
        return 1;
    }

    if (strcmp(name, "Dot") == 0){
        lua_pushcfunction(L, f_vector_dot, "Dot");
        return 1;
    }
    
    char err[1024] = {0};
    snprintf(err,1024,"%s is not a valid member of vector", name);
    lua_pushstring(L,err);
    lua_error(L);
}

static int f_vector_namecall(lua_State* L)
{
    const char* str = lua_namecallatom(L, NULL);
    if (str != NULL)
    {
        if (strcmp(str, "add") == 0)
            return f_vector_add(L);
        if (strcmp(str, "Dot") == 0)
            return f_vector_dot(L);
        if (strcmp(str, "2Dot") == 0)
            return f_vector_2dot(L);
    }
    char err[1024] = {0};
    snprintf(err,1024,"%s is not a valid method of Vector4", luaL_checkstring(L, 1));
    lua_pushstring(L,err);
    lua_error(L);
}


static const luaL_Reg lib[] = {
  { "new",f_vector_new},
  { NULL, NULL }
};

int luaopen_lmath(lua_State *L){
    lua_newtable(L);
    luaL_register(L,NULL,lib);
    lua_setglobal(L,"Vector4");

    lua_pushvector(L, 0.0f, 0.0f, 0.0f, 0.0f);
    luaL_newmetatable(L, "Vector4");

    lua_pushstring(L, "__index");
    lua_pushcfunction(L, f_vector_index, NULL);
    lua_settable(L, -3);

    lua_pushstring(L, "__namecall");
    lua_pushcfunction(L, f_vector_namecall, NULL);
    lua_settable(L, -3);

    lua_setmetatable(L, -2);
    
    lua_pop(L, 1);
}