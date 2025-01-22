cmake_minimum_required(VERSION 3.0)

set(CMAKE_CPP_COMPILER "clang++")

project(Drawing)

file(GLOB_RECURSE sources      src/*.cpp src/*.h)

add_executable(drawing.exe ${sources})

target_compile_options(bin.out PUBLIC -std=c++23)
