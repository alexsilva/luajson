#include "json.h"
#include "json-builder.h"

#include "lua.h"
#include "lauxlib.h"
#include "lapi.h"

#include "stdio.h"

#include "ljson.h"
#include "helpers.h"

static void convert_value(json_value *data, ObjIndex *objIndex);
static json_value *encode_array(lua_Object *obj, const char *key, json_value *object);
static json_value *encode_object(lua_Object *obj, const char *key, json_value *object);


static void new_object(json_value *data, ObjIndex *objIndex) {
    lua_Object obj = lua_createtable();

    if (objIndex->obj != NULL)
        key_object_pair(objIndex, &obj);

    int i;
    ObjIndex idx;
    idx.obj = &obj;
    for (i = 0; i < data->u.object.length; i++) {
        idx.key = data->u.object.values[i].name;
        idx.index = 0;
        convert_value(data->u.object.values[i].value, &idx);
    }
    idx.obj = NULL; // free pointer
}

static void new_array(json_value *data, ObjIndex *objIndex) {
    ObjIndex idx;
    lua_Object obj = lua_createtable();
    idx.obj = &obj;
    idx.key = NULL;
    idx.index = 1;

    if (objIndex->obj != NULL)
        key_object_pair(objIndex, idx.obj);

    int i;
    for (i = 0; i < data->u.array.length; i++) {
        convert_value(data->u.array.values[i], &idx);
    }
    idx.obj = NULL; // free pointer
}

static void convert_value(json_value *data, ObjIndex *objIndex){
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

static void decode(char *data, ObjIndex *objIndex) {
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

static bool is_indexed_array(lua_Object *obj) {
    lua_beginblock();
    int index = 0;
    index = lua_next(*obj, index);
    while (index != 0) {
        if (!lua_isnumber(lua_getparam(1)))
            return false;
        index = lua_next(*obj, index);
    }
    lua_endblock();
    return true;
}

static bool is_empty_array(lua_Object *obj) {
    lua_beginblock();
    int index = 0;
    index = lua_next(*obj, index);
    lua_endblock();
    return index == 0 ? true : false;
}

static json_value *encode_value(json_value *object, const char *key, lua_Object *value) {
    switch (luaA_Address(*value)->ttype) {
        case LUA_T_NUMBER:
            return json_double_new(lua_getnumber(*value));
        case LUA_T_STRING:
            return json_string_new(lua_getstring(*value));
        case LUA_T_ARRAY:
            return is_indexed_array(value) && !is_empty_array(value) ?
                   encode_array(value, key, object) : encode_object(value, key, object);
        case LUA_T_NIL:
            return json_null_new();
        default:
            lua_error("type not recognized!");
    }
    return NULL;
}


static json_value *encode_array(lua_Object *obj, const char *key, json_value *object) {
    lua_beginblock();
    json_value *arr = json_array_new(0);

    int index = 0;
    index = lua_next(*obj, index);

    lua_Object value;
    while (index != 0) {
        lua_getparam(1);
        value = lua_getparam(2);
        json_array_push(arr, encode_value(arr, key, &value));  // ex {1 = ?}
        index = lua_next(*obj, index);
    }
    lua_endblock();
    return arr;
}

static json_value *encode_object(lua_Object *obj, const char *key, json_value *object) {
    lua_beginblock();
    json_value *json_obj = json_object_new(0);

    int index = 0;
    index = lua_next(*obj, index);

    char *local_key;
    lua_Object value;
    while (index != 0) {
        local_key = lua_getstring(lua_getparam(1));
        value = lua_getparam(2);
        json_object_push(json_obj, local_key, encode_value(json_obj, local_key, &value));  // ex {a = ?}
        index = lua_next(*obj, index);
    }
    lua_endblock();
    return json_obj;
}

static void decodeJson(void) {
    ObjIndex idx;
    idx.obj = NULL;
    idx.key = NULL;
    idx.index = 1;
    char *str = luaL_check_string(1);
    decode(str, &idx);
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
        json_vl = !is_empty_array(&obj) ? encode_array(&obj, NULL, NULL) : encode_object(&obj, NULL, NULL);
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
