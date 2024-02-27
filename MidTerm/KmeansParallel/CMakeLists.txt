cmake_minimum_required(VERSION 3.27)
project(KmeansParallel)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fopenmp")

set(SOURCE_FILES main.cpp)
add_executable(KmeansParallel main.cpp)
