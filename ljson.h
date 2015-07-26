#ifndef __MAIN_H__
#define __MAIN_H__

#if defined(_MSC_VER)
    //  Microsoft
    #define LUA_LIBRARY __declspec(dllexport)
#else
    //  GCC
    #define LUA_LIBRARY __attribute__((visibility("default")))
#endif

#ifdef __cplusplus
extern "C"
{
#endif
#include "lua.h"
#include "lauxlib.h"
#include "lua_api.h"

typedef enum { false, true } bool;

struct OIdx {
    lua_Object *obj;
    char *key;
    int index;
};

typedef struct OIdx ObjIndex;

int LUA_LIBRARY lua_ljsonopen(lua_State *L);

#ifdef __cplusplus
}
#endif

#endif // __MAIN_H__
