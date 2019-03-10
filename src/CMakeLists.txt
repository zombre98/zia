cmake_minimum_required(VERSION 3.9)


set(CMAKE_CXX_STANDARD 17)
set(TARGET_NAME ${PROJECT_NAME})
include(CMakeSources.cmake)

include_directories(
        ${PROJECT_SOURCE_DIR}/asio
        ${PROJECT_SOURCE_DIR}/../../external/asio/include
)


include(${CMAKE_BINARY_DIR}/conan-${CONAN_CMAKE_VER}.cmake)


add_executable(${PROJECT_NAME} ${SRCS})

target_include_directories(${TARGET_NAME} PRIVATE lib/asio/include)
target_include_directories(${TARGET_NAME} PRIVATE src)

if (APPLE)
    target_link_directories(${TARGET_NAME} PRIVATE /usr/local/opt/llvm/lib)
    target_link_libraries(${TARGET_NAME} PRIVATE c++fs)
elseif(UNIX)
    target_link_libraries(${TARGET_NAME} PRIVATE stdc++fs)
    target_link_libraries(${TARGET_NAME} PRIVATE dl)
    target_link_libraries(${TARGET_NAME} PRIVATE pthread)
endif()

target_link_libraries(${PROJECT_NAME} PRIVATE ${CONAN_LIBS})

set_target_properties(${PROJECT_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

add_subdirectory(server/Modules)
