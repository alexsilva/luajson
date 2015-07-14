#ifndef __MAIN_H__
#define __MAIN_H__

#define DLL_EXPORT __declspec(dllexport)

#ifdef __cplusplus
extern "C"
{
#endif

typedef enum { false, true } bool;

struct OIdx {
    lua_Object *obj;
    char *key;
    int index;
};

typedef struct OIdx ObjIndex;

int DLL_EXPORT lua_ljsonopen(lua_State *L);

#ifdef __cplusplus
}
#endif

#endif // __MAIN_H__
