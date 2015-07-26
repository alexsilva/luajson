#include "json.h"
#include "json-builder.h"
#include "ljson.h"
#include "helpers.h"


static void convert_value(lua_State *L, json_value *data, ObjIndex *objIndex);
static json_value *encode_array(lua_State *L, lua_Object *obj, const char *key, json_value *object);
static json_value *encode_object(lua_State *L, lua_Object *obj, const char *key, json_value *object);


static void new_object(lua_State *L, json_value *data, ObjIndex *objIndex) {
    lua_Object obj = lua_createtable(L);

    if (objIndex->obj != NULL)
        key_object_pair(L, objIndex, &obj);

    int i;
    ObjIndex idx;
    idx.obj = &obj;
    for (i = 0; i < data->u.object.length; i++) {
        idx.key = data->u.object.values[i].name;
        idx.index = 0;
        convert_value(L, data->u.object.values[i].value, &idx);
    }
    idx.obj = NULL; // free pointer
    lua_pushobject(L, obj);
}

static void new_array(lua_State *L, json_value *data, ObjIndex *objIndex) {
    ObjIndex idx;
    lua_Object obj = lua_createtable(L);
    idx.obj = &obj;
    idx.key = NULL;
    idx.index = 1;

    if (objIndex->obj != NULL)
        key_object_pair(L, objIndex, idx.obj);

    int i;
    for (i = 0; i < data->u.array.length; i++) {
        convert_value(L, data->u.array.values[i], &idx);
    }
    idx.obj = NULL; // free pointer
    lua_pushobject(L, obj);
}

static void convert_value(lua_State *L, json_value *data, ObjIndex *objIndex){
    switch (data->type) {
        case json_object:
            new_object(L, data, objIndex);
            break;
        case json_array:
            new_array(L, data, objIndex);
            break;
        case json_integer:
            key_value_pair(L, objIndex, (double) data->u.integer);
            break;
        case json_double:
            key_value_pair(L, objIndex, data->u.dbl);
            break;
        case json_string:
            key_strlvalue_pair(L, objIndex, data->u.string.ptr,
                               (long) data->u.string.length);
            break;
        case json_boolean:
            key_value_pair(L, objIndex, data->u.boolean);
            break;
        default:
            // covers json_null, json_none
            key_nil_pair(L, objIndex);
            break;
    }
}

static void decode(lua_State *L, char *data, ObjIndex *objIndex) {
    json_settings settings;
    memset(&settings, 0, sizeof (json_settings));
    settings.settings = json_enable_comments;
    char error[256];
    json_value *value = json_parse_ex(&settings, data, strlen(data), error);
    if (value == 0) {
        return lua_error(L, (char *) error);
    }
    convert_value(L, value, objIndex);
    json_value_free(value);
}

static bool is_indexed_array(lua_State *L, lua_Object *obj) {
    lua_beginblock(L);
    int index = 0;
    index = lua_api_next(L, *obj, index);
    bool status = true;
    while (index != 0) {
        if (!lua_isnumber(L, lua_getparam(L, 1))) {
            status = false;
            break;
        }
        index = lua_api_next(L, *obj, index);
    }
    lua_endblock(L);
    return status;
}

static bool is_empty_array(lua_State *L, lua_Object *obj) {
    lua_beginblock(L);
    int index = 0;
    index = lua_api_next(L, *obj, index);
    lua_endblock(L);
    return index == 0 ? true : false;
}

static json_value *encode_value(lua_State *L, json_value *object, const char *key, lua_Object *value){
    if (lua_isnumber(L, *value)) {
        return json_double_new(lua_getnumber(L, *value));
    } else if (lua_isstring(L, *value)) {
        return json_string_new(lua_getstring(L, *value));
    } else if (lua_istable(L, *value)) {
        return is_indexed_array(L, value) && !is_empty_array(L, value) ?
               encode_array(L, value, key, object) : encode_object(L, value, key, object);
    } else if (lua_isnil(L, *value)) {
        return json_null_new();
    } else {
        lua_error(L, "Type not recognized!");
        return NULL;
    }
}


static json_value *encode_array(lua_State *L, lua_Object *obj, const char *key, json_value *object) {
    lua_beginblock(L);
    json_value *arr = json_array_new(0);

    int index = 0;
    index = lua_api_next(L, *obj, index);

    lua_Object value;
    while (index != 0) {
        lua_getparam(L, 1);
        value = lua_getparam(L, 2);
        json_array_push(arr, encode_value(L, arr, key, &value));  // ex {1 = ?}
        index = lua_api_next(L, *obj, index);
    }
    lua_endblock(L);
    return arr;
}

static json_value *encode_object(lua_State *L, lua_Object *obj, const char *key, json_value *object) {
    lua_beginblock(L);
    json_value *json_obj = json_object_new(0);

    int index = 0;
    index = lua_api_next(L, *obj, index);

    char *local_key;
    lua_Object value;
    while (index != 0) {
        local_key = lua_getstring(L, lua_getparam(L, 1));
        value = lua_getparam(L, 2);
        json_object_push(json_obj, local_key, encode_value(L, json_obj, local_key, &value));  // ex {a = ?}
        index = lua_api_next(L, *obj, index);
    }
    lua_endblock(L);
    return json_obj;
}

static void decodeJson(lua_State *L) {
    ObjIndex idx;
    idx.obj = NULL;
    idx.key = NULL;
    idx.index = 1;
    char *str = luaL_check_string(L, 1);
    decode(L, str, &idx);
}

static void encodeJson(lua_State *L) {
    lua_Object obj = lua_getparam(L, 1);
    json_value *json_vl;
    if (lua_isnumber(L, obj)) {
        json_vl = json_double_new(lua_getnumber(L, obj)); // number like 1
    } else if (lua_isstring(L, obj)) {
        json_vl = json_string_new(lua_getstring(L, obj)); // string like "a" or "\"\""
    } else if (lua_isnil(L, obj)) {
        json_vl = json_null_new(); // null like nil
    } else if (is_indexed_array(L, &obj) == true) {  // array [1,3,4]
        json_vl = !is_empty_array(L, &obj) ? encode_array(L, &obj, NULL, NULL) : encode_object(L, &obj, NULL, NULL);
    } else {
        json_vl = encode_object(L, &obj, NULL, NULL);  // object {"a": 1}
    }
    char *buf = malloc(json_measure(json_vl));
    json_serialize(buf, json_vl);

    lua_pushstring(L, buf);
    free(buf);
}

static struct luaL_reg json[] = {
    {"json_decode", decodeJson},
    {"json_encode", encodeJson}
};


int DLL_EXPORT lua_ljsonopen(lua_State *L) {
    luaL_openlib(L, json, (sizeof(json)/sizeof(json[0])));
    return 0;
}
