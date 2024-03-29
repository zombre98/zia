cmake_minimum_required(VERSION 3.9)
project(ZiaModules)

if (APPLE)
    set(CMAKE_C_COMPILER "/usr/local/opt/llvm/bin/clang")
    set(CMAKE_CXX_COMPILER "/usr/local/opt/llvm/bin/clang++")
endif()

set(CMAKE_CXX_STANDARD 17)

add_library(${PROJECT_NAME} SHARED
        LoadPathModule.cpp)

add_library(PHPModule SHARED
            PHPModule.cpp)

if (APPLE)
    target_link_directories(${PROJECT_NAME} PRIVATE /usr/local/opt/llvm/lib)
    target_link_libraries(${PROJECT_NAME} PRIVATE c++fs)
    target_link_directories(PHPModule PRIVATE /usr/local/opt/llvm/lib)
    target_link_libraries(PHPModule PRIVATE c++fs)
elseif(UNIX)
    target_link_libraries(${PROJECT_NAME} PRIVATE stdc++fs)
    target_link_libraries(${PROJECT_NAME} PRIVATE dl)
    target_link_libraries(${PROJECT_NAME} PRIVATE pthread)
endif()
