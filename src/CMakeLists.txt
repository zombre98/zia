cmake_minimum_required(VERSION 3.9)

project(server)

set(CMAKE_CXX_STANDARD 17)


include(CMakeSources.cmake)

include_directories(
        ${PROJECT_SOURCE_DIR}/asio
        ${PROJECT_SOURCE_DIR}/../../external/asio/include
)

add_executable(${PROJECT_NAME} ${SRCS})

#target_include_directories(${PROJECT_NAME} PRIVATE include)
#target_include_directories(${PROJECT_NAME} PRIVATE include/Audio)
#target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/common)
#target_include_directories(${PROJECT_NAME} PRIVATE src/Qt)

#target_link_libraries(${PROJECT_NAME} ${CONAN_LIBS})

#target_link_libraries(${PROJECT_NAME} ${CONAN_LIBS} Qt5::Widgets Qt5::Gui Qt5::Core Qt5::Network)

#set_target_properties(${PROJECT_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})