cmake_minimum_required(VERSION 3.10)
set(CMAKE_C_COMPILER "/home/christian/Code/learning-c/gbdk/bin/lcc")
project(game C)

add_executable(game main.c lib/gb_headers.h lib/turing_logic.c lib/turing_machine.h
        lib/turing_init.c)

set(CMAKE_C_STANDARD 11)
set(GLOBAL_INCLUDE_DIRS
        /usr/include/CUnit
)
include_directories(${GLOBAL_INCLUDE_DIRS})

target_include_directories(game PUBLIC ${GLOBAL_INCLUDE_DIRS} gbdk/include)

add_subdirectory(tests)