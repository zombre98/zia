cmake_minimum_required(VERSION 3.9)

project(serverzia)

set(CMAKE_CXX_STANDARD 17)
set(TARGET_NAME ${PROJECT_NAME})
include(CMakeSources.cmake)

include_directories(
        ${PROJECT_SOURCE_DIR}/asio
        ${PROJECT_SOURCE_DIR}/../../external/asio/include
)

set(CONAN_CMAKE_VER v0.13)
if(NOT EXISTS "${CMAKE_BINARY_DIR}/conan-${CONAN_CMAKE_VER}.cmake")
   message(STATUS "Downloading conan.cmake from https://github.com/conan-io/cmake-conan")
   file(DOWNLOAD "https://raw.githubusercontent.com/conan-io/cmake-conan/${CONAN_CMAKE_VER}/conan.cmake"
       "${CMAKE_BINARY_DIR}/conan-${CONAN_CMAKE_VER}.cmake")
endif()

include(${CMAKE_BINARY_DIR}/conan-${CONAN_CMAKE_VER}.cmake)

if(EXISTS ${CMAKE_CURRENT_BINARY_DIR}/conanbuildinfo.cmake)
   set(CONAN_EXPORTED)
   set(CMAKE_BUILD_TYPE Debug)
endif()

add_executable(${PROJECT_NAME} ${SRCS} Utils/Utils.hpp)

target_include_directories(${TARGET_NAME} PRIVATE lib/asio/include)
target_include_directories(${TARGET_NAME} PRIVATE src)

if (APPLE)
    target_link_directories(${TARGET_NAME} PRIVATE /usr/local/opt/llvm/lib)
    target_link_libraries(${TARGET_NAME} PRIVATE c++fs)
else()
    target_link_libraries(${TARGET_NAME} PRIVATE stdc++fs)
    target_link_libraries(${TARGET_NAME} PRIVATE dl)
    target_link_libraries(${TARGET_NAME} PRIVATE pthread)
endif()

target_link_libraries(${PROJECT_NAME} PRIVATE ${CONAN_LIBS})
#target_link_libraries(${PROJECT_NAME} ${CONAN_LIBS})

set_target_properties(${PROJECT_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
