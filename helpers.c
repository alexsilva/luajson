#include "json.h"

#include "lua.h"
#include "ljson.h"


void set_key_ref(lua_State *L, ObjIndex *objIdx) {
    if (objIdx->key != NULL) {
        lua_pushstring(L, objIdx->key);
    } else {
        lua_pushnumber(L, objIdx->index);
        objIdx->index++;
    }
}

void key_strvalue_pair(lua_State *L, ObjIndex *objIdx, char *str) {
    if (objIdx->obj != NULL) {
        lua_pushobject(L, *objIdx->obj);
        set_key_ref(L, objIdx);
    }
    lua_pushstring(L, str);

    if (objIdx->obj != NULL) {
        lua_settable(L);
    }
}

void key_strlvalue_pair(lua_State *L, ObjIndex *objIdx, char *str, long length) {
    if (objIdx->obj != NULL) {
        lua_pushobject(L, *objIdx->obj);
        set_key_ref(L, objIdx);
    }
    lua_pushlstring(L, str, length);

    if (objIdx->obj != NULL) {
        lua_settable(L);
    }
}

void key_nil_pair(lua_State *L, ObjIndex *objIdx) {
    if (objIdx->obj != NULL) {
        lua_pushobject(L, *objIdx->obj);
        set_key_ref(L, objIdx);
    }
    lua_pushnil(L);

    if (objIdx->obj != NULL) {
        lua_settable(L);
    }
}

void key_value_pair(lua_State *L, ObjIndex *objIdx, double number) {
    if (objIdx->obj != NULL) {
        lua_pushobject(L, *objIdx->obj);
        set_key_ref(L, objIdx);
    }
    lua_pushnumber(L, number);

    if (objIdx->obj != NULL) {
        lua_settable(L);
    }
}

void key_object_pair(lua_State *L, ObjIndex *objIdx, lua_Object *obj) {
    if (objIdx->obj != NULL) {
        lua_pushobject(L, *objIdx->obj);
        set_key_ref(L, objIdx);
    }
    lua_pushobject(L, *obj);

    if (objIdx->obj != NULL) {
        lua_settable(L);
        lua_pushobject(L, *objIdx->obj);
    }
}
