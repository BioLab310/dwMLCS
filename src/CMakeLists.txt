cmake_minimum_required(VERSION 3.21.1)

project(dwMLCS)

set(CMAKE_CXX_STANDARD 11)

add_executable(dwMLCS basic.h basic.cpp lower.h lower.cpp upper.h upper.cpp dwMLCS.h dwMLCS.cpp main.cpp)
