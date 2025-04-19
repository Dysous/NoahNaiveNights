cmake_minimum_required(VERSION 3.27)
project(TigerGame)

set(CMAKE_CXX_STANDARD 17)

add_executable(TigerGame main.cpp
        Tiger.h
        Soldiers.cpp
        Tiger.cpp)
