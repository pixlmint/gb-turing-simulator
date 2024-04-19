cmake_minimum_required(VERSION 3.10)
set(CMAKE_C_COMPILER "/home/christian/Downloads/gbdk/bin/lcc")
project(game C)

add_executable(game main.c lib/gb_headers.h lib/turing_logic.c lib/turing_machine.h
        lib/turing_init.c)

set(CMAKE_C_STANDARD 11)
set(GLOBAL_INCLUDE_DIRS
        #/home/christian/Downloads/gbdk/include
        /usr/include/CUnit
)
include_directories(${GLOBAL_INCLUDE_DIRS})

project(tests C)
set(CMAKE_C_COMPILER "/usr/bin/gcc")
add_definitions(-D__PORT_z80)

add_executable(tests tests/test.c)
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_C_FLAGS_DEBUG "-g -O0")

target_include_directories(tests PUBLIC ${GLOBAL_INCLUDE_DIRS})
target_include_directories(game PUBLIC ${GLOBAL_INCLUDE_DIRS} /home/christian/Downloads/gbdk/include)

target_link_libraries(tests cunit)
