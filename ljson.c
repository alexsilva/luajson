#include "json.h"
#include "json-builder.h"

#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
#include "lapi.h"

#include <stdlib.h>
#include "stdio.h"

#include "ljson.h"
#include "helpers.h"

json_value *encode_array(lua_Object *obj, const char *key, json_value *object);
json_value *encode_object(lua_Object *obj, const char *key, json_value *object);


void new_object(json_value *data, ObjIndex *objIndex) {
    lua_Object obj = lua_createtable();

    if (objIndex->obj)
        key_object_pair(objIndex, &obj);

    int i;
    for (i = 0; i < data->u.object.length; i++) {
        ObjIndex idx;

        idx.obj = &obj;
        idx.key = data->u.object.values[i].name;
        idx.index = 0;

        convert_value(data->u.object.values[i].value, &idx);
        idx.obj = NULL; // free pointer
    }
}

void new_array(json_value *data, ObjIndex *objIndex) {
    ObjIndex idx;
    lua_Object obj = lua_createtable();
    idx.obj = &obj;
    idx.key = NULL;
    idx.index = 1;

    if (objIndex->obj)
        key_object_pair(objIndex, idx.obj);

    int i;
    for (i = 0; i < data->u.array.length; i++) {
        convert_value(data->u.array.values[i], &idx);
    }
    idx.obj = NULL; // free pointer
}

void convert_value(json_value *data, ObjIndex *objIndex){
    switch (data->type) {
        case json_object:
            new_object(data, objIndex);
            break;
        case json_array:
            new_array(data, objIndex);
            break;
        case json_integer:
            key_value_pair(objIndex, (double) data->u.integer);
            break;
        case json_double:
            key_value_pair(objIndex, data->u.dbl);
            break;
        case json_string:
            key_strlvalue_pair(objIndex, data->u.string.ptr,
                               (long) data->u.string.length);
            break;
        case json_boolean:
            key_value_pair(objIndex, data->u.boolean);
            break;
        default:
            // covers json_null, json_none
            key_nil_pair(objIndex);
            break;
    }
}

void decode(char *data, ObjIndex *objIndex) {
    json_settings settings;
    memset(&settings, 0, sizeof (json_settings));
    settings.settings = json_enable_comments;
    char error[256];
    json_value *value = json_parse_ex(&settings, data, strlen(data), error);
    if (value == 0) {
        return lua_error((char *) error);
    }
    convert_value(value, objIndex);
    json_value_free(value);
}

bool is_indexed_array(lua_Object *obj) {
    int index = 0;
    index = lua_next(*obj, index);
    while (index != 0) {
        if (!lua_isnumber(lua_getparam(1)))
            return false;
        index = lua_next(*obj, index);
    }
    return true;
}

json_value *encode_value(json_value *object, const char *key, lua_Object *value) {
    switch (luaA_Address(*value)->ttype) {
        case LUA_T_NUMBER:
            return json_double_new(lua_getnumber(*value));
        case LUA_T_STRING:
            return json_string_new(lua_getstring(*value));
        case LUA_T_ARRAY:
            return is_indexed_array(value) ? encode_array(value, key, object) : encode_object(value, key, object);
        case LUA_T_NIL:
            return json_null_new();
        default:
            lua_error("type not recognized!");
    }
    return NULL;
}


json_value *encode_array(lua_Object *obj, const char *key, json_value *object) {
    json_value *arr = json_array_new(0);
    if (object != NULL) {
        switch (object->type) {
            case json_array:
                json_array_push(object, arr);
                break;
            default:  // is object
                json_object_push(object, key,  arr);
        }
    }
    int index = 0;
    index = lua_next(*obj, index);

    while (index != 0) {
        lua_Object value = lua_getparam(2);
        json_array_push(arr, encode_value(object, key, &value));  // ex {1 = ?}
        index = lua_next(*obj, index);
    }
    return arr;
}

json_value *encode_object(lua_Object *obj, const char *key, json_value *object) {
    json_value *arr = json_object_new(0);
    if (object != NULL) {
        switch (object->type) {
            case json_array:
                json_array_push(object, arr);
                break;
            default:  // is object
                json_object_push(object, key,  arr);
        }
    }
    int index = 0;
    index = lua_next(*obj, index);

    while (index != 0) {
        char *local_key = lua_getstring(lua_getparam(1));
        lua_Object value = lua_getparam(2);

        json_object_push(arr, local_key, encode_value(arr, local_key, &value));  // ex {a = ?}

        index = lua_next(*obj, index);
    }
    return arr;
}

static void decodeJson(void) {
    ObjIndex idx;
    idx.obj = NULL;
    idx.key = NULL;
    idx.index = 1;
    decode(luaL_check_string(1), &idx);
}

static void encodeJson(void) {
    lua_Object obj = lua_getparam(1);
    json_value *json_vl;

    if (lua_isnumber(obj)) {
        json_vl = json_double_new(lua_getnumber(obj)); // number like 1
    } else if (lua_isstring(obj)) {
        json_vl = json_string_new(lua_getstring(obj)); // string like "a" or "\"\""
    } else if (lua_isnil(obj)) {
        json_vl = json_null_new(); // null like nil
    } else if (is_indexed_array(&obj) == true) {  // array [1,3,4]
        json_vl = encode_array(&obj, NULL, NULL);
    } else {
        json_vl = encode_object(&obj, NULL, NULL);  // object {"a": 1}
    }
    char *buf = malloc(json_measure(json_vl));
    json_serialize(buf, json_vl);

    lua_pushstring(buf);
    free(buf);
}

static struct luaL_reg json[] = {
    {"json_decode", decodeJson},
    {"json_encode", encodeJson}
};

int DLL_EXPORT lua_ljsonopen(lua_State *state)
{
    lua_state = state;

    luaL_openlib(json, (sizeof(json)/sizeof(json[0])));
    return 0;
}
