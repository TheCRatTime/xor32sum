cmake_minimum_required(VERSION 3.10)

project(xor32sum C)

set(CMAKE_C_STANDARD 90)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

include_directories(include)
add_executable(xor32sum src/xor32sum.c src/xorshift32.c)
