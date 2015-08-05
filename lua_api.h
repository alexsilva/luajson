//
// Created by alex on 14/07/2015.
//

#ifndef LUAJSON_LUA_API_H
#define LUAJSON_LUA_API_H

#include "lua.h"

#define lua_api_address(L, lo) ((lo)+L->stack.stack-1)

int lua_api_next (lua_State *L, lua_Object o, int i);

#endif //LUAJSON_LUA_API_H
