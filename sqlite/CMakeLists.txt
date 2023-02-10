cmake_minimum_required (VERSION 3.19)

file(GLOB_RECURSE C_FILES sqlite3.c)
file(GLOB_RECURSE H_FILES sqlite3.h)

add_library(sqlite STATIC ${C_FILES} ${H_FILES})

add_library(sqlite::sqlite ALIAS sqlite)