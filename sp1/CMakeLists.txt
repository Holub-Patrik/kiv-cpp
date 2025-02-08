cmake_minimum_required(VERSION 3.1)

set(CMAKE_CPP_COMPILER "g++")

project(Drawing)

file(GLOB_RECURSE sources      src/*.cpp src/*.h)

add_executable(drawing.exe ${sources})

target_compile_options(drawing.exe PUBLIC -std=c++23)
