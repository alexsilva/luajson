#include "json.h"

#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
#include "ljson.h"


void set_key_ref(ObjIndex *objIdx) {
    if (objIdx->key != NULL) {
        lua_pushstring(objIdx->key);
    } else {
        lua_pushnumber(objIdx->index);
        objIdx->index++;
    }
}

void key_strvalue_pair(ObjIndex *objIdx, char *str) {
    lua_pushobject(objIdx->obj);

    set_key_ref(objIdx);
    lua_pushstring(str);

    lua_settable();
    lua_pushobject(objIdx->obj);
}

void key_strlvalue_pair(ObjIndex *objIdx, char *str, long length) {
    lua_pushobject(objIdx->obj);

    set_key_ref(objIdx);
    lua_pushlstring(str, length);

    lua_settable();
    lua_pushobject(objIdx->obj);
}

void key_nil_pair(ObjIndex *objIdx) {
    lua_pushobject(objIdx->obj);

    set_key_ref(objIdx);
    lua_pushnil();

    lua_settable();
    lua_pushobject(objIdx->obj);
}

void key_value_pair(ObjIndex *objIdx, double number) {
    lua_pushobject(objIdx->obj);

    set_key_ref(objIdx);
    lua_pushnumber(number);

    lua_settable();
    lua_pushobject(objIdx->obj);
}

void key_object_pair(ObjIndex *objIdx, lua_Object *obj) {
    lua_pushobject(objIdx->obj);

    set_key_ref(objIdx);
    lua_pushobject(*obj);

    lua_settable();
    lua_pushobject(objIdx->obj);
}
