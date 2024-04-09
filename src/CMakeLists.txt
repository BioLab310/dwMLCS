# Created by D_Kuan

cmake_minimum_required(VERSION 3.13.3)
project(dwMLCS)

set(CMAKE_CXX_STANDARD 11)

add_executable(dwMLCS basic.h basic.cpp lower.h lower.cpp upper.h upper.cpp dwMLCS.h dwMLCS.cpp main.cpp)
