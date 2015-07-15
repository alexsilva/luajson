#ifndef HELPERS_H_INCLUDED
#define HELPERS_H_INCLUDED


void key_strvalue_pair(lua_State *L, ObjIndex *objIdx, char *str);
void key_strlvalue_pair(lua_State *L, ObjIndex *objIdx, char *str, long length);
void key_value_pair(lua_State *L, ObjIndex *objIdx, double number);
void key_object_pair(lua_State *L, ObjIndex *objIdx, lua_Object *obj);
void key_nil_pair(lua_State *L, ObjIndex *objIdx);

#endif // HELPERS_H_INCLUDED
