cmake_minimum_required(VERSION 3.9)
project(ZiaModules)

set(CMAKE_CXX_STANDARD 17)

add_library(${PROJECT_NAME} SHARED
        LoadPathModule.cpp)
