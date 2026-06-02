cmake_minimum_required(VERSION 3.10)
project(cdict)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

add_executable(cdict main.c dict.c)