//
// Created by alex on 14/07/2015.
//

#include <ldo.h>
#include "lua_api.h"
#include "ltable.h"

/*
** generic allocation routine.
*/
void luaD_checkstack (lua_State *L, int n) {}

void lapi_pushobject(lua_State *L, TObject *o) {
    *L->stack.top = *o;
    incr_top;
}

static void lua_api_top2L(lua_State *L, int n) {
    /* Put the 'n' elements on the top as the Lua2C contents */
    L->Cstack.base = (L->stack.top-L->stack.stack);  /* new base */
    L->Cstack.lua2C = L->Cstack.base-n;  /* position of the new results */
    L->Cstack.num = n;  /* number of results */
}

int lua_apiA_next(lua_State *L, Hash *t, int i) {
    int tsize = nhash(L, t);
    for (; i < tsize; i++) {
        Node *n = node(L, t, i);
        if (ttype(val(L, n)) != LUA_T_NIL) {
            lapi_pushobject(L, ref(L, n));
            lapi_pushobject(L, val(L, n));
            return i+1;  /* index to be used next time */
        }
    }
    return 0;  /* no more elements */
}

int lua_api_next(lua_State *L, lua_Object o, int i) {
    TObject *t = lua_api_address(L, o);
    if (ttype(t) != LUA_T_ARRAY)
        lua_error(L, "API error - object is not a table in `lua_next'");
    i = lua_apiA_next(L, avalue(t), i);
    lua_api_top2L(L, (i==0) ? 0 : 2);
    return i;
}