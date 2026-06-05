cmake_minimum_required(VERSION 3.10)
project(cdict)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(cdict main.cxx dict.c)

if(MSVC)
    target_compile_options(cdict PRIVATE /O2 /W4)
else()
    target_compile_options(cdict PRIVATE -O2 -Wall -Wextra -pedantic)
endif()

# EOF