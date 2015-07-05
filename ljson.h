#ifndef __MAIN_H__
#define __MAIN_H__

#include <windows.h>

/*  To use this exported function of dll, include this header
 *  in your project.
 */

#ifdef BUILD_DLL
    #define DLL_EXPORT __declspec(dllexport)
#else
    #define DLL_EXPORT __declspec(dllimport)
#endif


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

int DLL_EXPORT lua_ljsonopen(lua_State *state);

#ifdef __cplusplus
}
#endif

#endif // __MAIN_H__
