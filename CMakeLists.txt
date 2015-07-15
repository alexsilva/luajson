cmake_minimum_required(VERSION 3.2)
project(luajson)

# [EXTERNAL OPTION] -D LUA_DIR=../Lua3.2
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

include_directories(
    ${LUA_DIR}/include
    ${LUA_DIR}/src
    "json-builder"
    "json-parser"
)

set(SOURCE_FILES
    json-builder/json-builder.c
    json-builder/json-builder.h
    json-parser/json.c
    json-parser/json.h
    lua_api.c
    lua_api.h
    helpers.c
    helpers.h
    ljson.c
    ljson.h)

add_library(ljson SHARED ${SOURCE_FILES})
target_link_libraries(ljson ${LUA_LIBRARIES})