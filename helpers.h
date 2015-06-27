#ifndef HELPERS_H_INCLUDED
#define HELPERS_H_INCLUDED


void key_strvalue_pair(ObjIndex *objIdx, char *str);
void key_strlvalue_pair(ObjIndex *objIdx, char *str, long length);
void key_value_pair(ObjIndex *objIdx, double number);
void key_object_pair(ObjIndex *objIdx, lua_Object *obj);
void key_nil_pair(ObjIndex *objIdx);

#endif // HELPERS_H_INCLUDED
