cmake_minimum_required(VERSION 3.9)

if (APPLE)
    set(CMAKE_C_COMPILER "/usr/local/opt/llvm/bin/clang")
    set(CMAKE_CXX_COMPILER "/usr/local/opt/llvm/bin/clang++")
endif()

set(CMAKE_CXX_STANDARD 17)

add_library(LoadPathModule SHARED
        LoadPathModule.cpp)

add_library(PHPModule SHARED
            PHPModule.cpp)

add_library(OpenSSL SHARED
        openSslModule.cpp)

if (APPLE)
    target_link_directories(LoadPathModule PRIVATE /usr/local/opt/llvm/lib)
    target_link_libraries(LoadPathModule PRIVATE c++fs)
    target_link_directories(PHPModule PRIVATE /usr/local/opt/llvm/lib)
    target_link_libraries(PHPModule PRIVATE c++fs)
elseif(UNIX)
    target_link_libraries(LoadPathModule PRIVATE stdc++fs)
    target_link_libraries(LoadPathModule PRIVATE dl)
    target_link_libraries(LoadPathModule PRIVATE pthread)
endif()
target_link_libraries(OpenSSL PRIVATE ssl crypto)
target_link_libraries(OpenSSL PRIVATE boost_system)
