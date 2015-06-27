#include "json.h"

#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"

#include <stdlib.h>
#include "stdio.h"

#include "ljson.h"
#include "helpers.h"


void new_object(json_value *data, ObjIndex *objIndex) {
    lua_Object obj = lua_createtable();

    if (objIndex->obj != NULL)
        key_object_pair(objIndex, &obj);

    int i;
    for (i = 0; i < data->u.object.length; i++) {
        ObjIndex idx;

        idx.obj = obj;
        idx.key = data->u.object.values[i].name;
        idx.index = 0;

        convert_value(data->u.object.values[i].value, &idx);
    }
}

void new_array(json_value *data, ObjIndex *objIndex) {
    ObjIndex idx;
    idx.obj = lua_createtable();
    idx.key = "";
    idx.index = 1;

    if (objIndex->obj != NULL)
        key_object_pair(objIndex, &(idx.obj));

    int i;
    for (i = 0; i < data->u.array.length; i++) {
        convert_value(data->u.array.values[i], &idx);
    }
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
                               data->u.string.length);
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
        return lua_error(error);
    }
    convert_value(value, objIndex);
    json_value_free(value);
}


static void decodeJson(void) {
    ObjIndex idx;
    idx.obj = NULL;
    idx.key = "";
    idx.index = 0;
    decode(luaL_check_string(1), &idx);
}

static struct luaL_reg json[] = {
    {"json_decode", decodeJson}
};

int DLL_EXPORT lua_ljsonopen(lua_State *state)
{
    lua_state = state;

    luaL_openlib(json, (sizeof(json)/sizeof(json[0])));
    return 0;
}
