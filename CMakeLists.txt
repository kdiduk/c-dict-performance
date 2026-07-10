cmake_minimum_required(VERSION 3.10)
project(cdict)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(cdict
    src/benchmark.cpp
    src/benchmark.hpp
    src/main.cxx
    src/key_generators.cpp
    src/key_generators.hpp
    src/dict.c
    src/dict.h
    src/qdict.c
    src/qdict.h
    src/hmap.h
    src/hmap.c)

add_executable(hmap_test
    src/hmap.c
    src/hmap.h
    src/hmap_test.c)

if(MSVC)
    target_compile_options(cdict PRIVATE /O2 /W4)
else()
    target_compile_options(cdict PRIVATE -O2 -Wall -Wextra -pedantic)
endif()

# EOF
