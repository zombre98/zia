cmake_minimum_required(VERSION 3.9)
project(ZiaModules)

set(CMAKE_C_COMPILER "/usr/local/opt/llvm/bin/clang")
set(CMAKE_CXX_COMPILER "/usr/local/opt/llvm/bin/clang++")

set(CMAKE_CXX_STANDARD 17)

add_library(${PROJECT_NAME} SHARED
        LoadPathModule.cpp)

if (APPLE)
    target_link_directories(${PROJECT_NAME} PRIVATE /usr/local/opt/llvm/lib)
    target_link_libraries(${PROJECT_NAME} PRIVATE c++fs)
endif()

