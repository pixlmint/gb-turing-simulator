cmake_minimum_required(VERSION 3.10)
project(tests C)
set(CMAKE_C_COMPILER "/usr/bin/gcc")
add_definitions(-D__PORT_z80)

add_executable(tests test.c)
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_C_FLAGS_DEBUG "-g -O0")

include_directories(${GLOBAL_INCLUDE_DIRS})

target_include_directories(tests PUBLIC ${GLOBAL_INCLUDE_DIRS})
target_link_libraries(tests cunit)