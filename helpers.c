#include "json.h"

#include "lua.h"
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
    if (objIdx->obj != NULL) {
        lua_pushobject(*objIdx->obj);
        set_key_ref(objIdx);
    }
    lua_pushstring(str);

    if (objIdx->obj != NULL) {
        lua_settable();
    }
}

void key_strlvalue_pair(ObjIndex *objIdx, char *str, long length) {
    if (objIdx->obj != NULL) {
        lua_pushobject(*objIdx->obj);
        set_key_ref(objIdx);
    }
    lua_pushlstring(str, length);

    if (objIdx->obj != NULL) {
        lua_settable();
    }
}

void key_nil_pair(ObjIndex *objIdx) {
    if (objIdx->obj != NULL) {
        lua_pushobject(*objIdx->obj);
        set_key_ref(objIdx);
    }
    lua_pushnil();

    if (objIdx->obj != NULL) {
        lua_settable();
    }
}

void key_value_pair(ObjIndex *objIdx, double number) {
    if (objIdx->obj != NULL) {
        lua_pushobject(*objIdx->obj);
        set_key_ref(objIdx);
    }
    lua_pushnumber(number);

    if (objIdx->obj != NULL) {
        lua_settable();
    }
}

void key_object_pair(ObjIndex *objIdx, lua_Object *obj) {
    if (objIdx->obj != NULL) {
        lua_pushobject(*objIdx->obj);
        set_key_ref(objIdx);
    }
    lua_pushobject(*obj);

    if (objIdx->obj != NULL) {
        lua_settable();
        lua_pushobject(*objIdx->obj);
    }
}
