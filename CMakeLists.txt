# This file specifies how the project should be built, using CMake.
# If you are unfamiliar with CMake, don't worry about all the details.
# The sections you might want to edit are marked as such, and 
# the comments should hopefully make most of it clear.
#
# For many purposes, you may not need to change anything about this file.

cmake_minimum_required(VERSION 3.1.3)

# Set project name, version and laguages here. (change as needed)
# Version numbers are available by including "exampleConfig.h" in 
# the source. See exampleConfig.h.in for some more details.
project(zia VERSION 1.0 LANGUAGES C CXX)


# Include stuff. No change needed.
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")
include(ConfigSafeGuards)
include(Colors)


# --------------------------------------------------------------------------------
#                          Compile flags (change as needed).
# --------------------------------------------------------------------------------
# Set the C++ standard you wish to use (will apply to all files).
# If you do not use any features that limits the standard required,
# you could omit this line.
set(CMAKE_CXX_STANDARD 17)

# Things to always include as flags. Change as needed.
if (!WIN32)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")

    # Build-type specific flags. Change as needed.
    SET(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG")
    SET(CMAKE_CXX_FLAGS_DEBUG "-g -O0")
endif()

message(STATUS "Building with the following extra flags: ${CMAKE_CXX_FLAGS}")

# --------------------------------------------------------------------------------
#                         Locate files (no change needed).
# --------------------------------------------------------------------------------
# We make sure that CMake sees all the files.
include_directories(
    ${PROJECT_SOURCE_DIR}/src
    ${PROJECT_SOURCE_DIR}/include
    ${PROJECT_SOURCE_DIR}/tests
    ${PROJECT_SOURCE_DIR}/external/asio/include
    ${PROJECT_SOURCE_DIR}/external/googletest
    ${PROJECT_SOURCE_DIR}/external/googletest/include)

# Make variables referring to all the sources and test files.
file(GLOB SOURCES "src/*.cpp")
file(GLOB TESTFILES "tests/*.cpp")
set(TEST_MAIN unit_tests)  # Default name for test executable (change if you wish).



# --------------------------------------------------------------------------------
#                            Build! (Change as needed)
# --------------------------------------------------------------------------------
# Compile all sources into a library. Called engine here (change if you wish).
add_library(engine ${SOURCES})

set(CONAN_CMAKE_VER v0.13)
if(NOT EXISTS "${CMAKE_BINARY_DIR}/conan-${CONAN_CMAKE_VER}.cmake")
       message(STATUS "Downloading conan.cmake from https://github.com/conan-io/cmake-conan")
       file(DOWNLOAD "https://raw.githubusercontent.com/conan-io/cmake-conan/${CONAN_CMAKE_VER}/conan.cmake"
           "${CMAKE_BINARY_DIR}/conan-${CONAN_CMAKE_VER}.cmake")
endif()

include(${CMAKE_BINARY_DIR}/conan-${CONAN_CMAKE_VER}.cmake)

if(EXISTS ${CMAKE_CURRENT_BINARY_DIR}/conan-${CONAN_CMAKE_VER}.cmake)
   set(CONAN_EXPORTED)
   set(CMAKE_BUILD_TYPE Debug)
endif()

include(${CMAKE_CURRENT_BINARY_DIR}/conanbuildinfo.cmake)
include_directories(${CONAN_INCLUDE_DIRS})

# --------------------------------------------------------------------------------
#                         Make Tests (no change needed).
# --------------------------------------------------------------------------------
# Add a make target 'gtest', that runs the tests (and builds all dependencies).
# The setup of Google Test is done at the very end of this file.
add_executable(${TEST_MAIN} ${TESTFILES})
add_dependencies(${TEST_MAIN} googletest engine)
target_link_libraries(${TEST_MAIN} googletest engine pthread ${CONAN_LIBS})
add_custom_target(gtest 
    COMMAND "${PROJECT_BINARY_DIR}/${TEST_MAIN}"
    DEPENDS engine ${TEST_MAIN})


# Add a standard make target 'test' that runs the tests under CTest (only as an alt. to gtest).
include(CTest)
enable_testing()
add_test(unit_tests ${PROJECT_BINARY_DIR}/${TEST_MAIN})

# --------------------------------------------------------------------------------
#                         Code Coverage (no change needed).
# --------------------------------------------------------------------------------
# If CMake aborts due to missing dependencies for code coverage 
# (gcov, lcov, genhtml and supported compiler), comment this line.
include(CodeCoverage)

# --------------------------------------------------------------------------------
#                         Documentation (no change needed).
# --------------------------------------------------------------------------------
# Add a make target 'doc' to generate API documentation with Doxygen.
# You should set options to your liking in the file 'Doxyfile.in'.
find_package(Doxygen)
if(DOXYGEN_FOUND)
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile @ONLY)
    set(DOXYGEN_ENGINE Doxyfile)
    if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/external/m.css/doxygen/dox2html5.py)
        configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile-mcss ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile-mcss @ONLY)
        set(DOXYGEN_EXECUTABLE ${CMAKE_CURRENT_SOURCE_DIR}/external/m.css/doxygen/dox2html5.py)
        set(DOXYGEN_ENGINE Doxyfile-mcss)
    endif()
    add_custom_target(doc
        ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/${DOXYGEN_ENGINE} &> /dev/null
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        COMMENT "${BoldMagenta}Generating API documentation with ${DOXYGEN_ENGINE} (open ${CMAKE_CURRENT_BINARY_DIR}/doc/html/index.html to view).${ColourReset}" VERBATIM
        )
endif(DOXYGEN_FOUND)


# --------------------------------------------------------------------------------
#                         Google Test (no change needed).
# --------------------------------------------------------------------------------
# The following makes sure that an up-to-date version of googletest is available,
# and built so that it may be used by your tests. 
add_custom_target(git_update
    COMMAND git submodule init
    COMMAND git submodule update
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR} )
add_library( googletest
    ${PROJECT_SOURCE_DIR}/external/googletest/src/gtest-all.cc
    ${PROJECT_SOURCE_DIR}/external/googletest/src/gtest_main.cc )
add_dependencies(googletest git_update)
set_source_files_properties(${PROJECT_SOURCE_DIR}/external/googletest/src/gtest-all.cc  PROPERTIES GENERATED 1)
set_source_files_properties(${PROJECT_SOURCE_DIR}/external/googletest/src/gtest_main.cc PROPERTIES GENERATED 1)



# --------------------------------------------------------------------------------
#                            Misc (no change needed).
# --------------------------------------------------------------------------------
# Have CMake parse the config file, generating the config header, with
# correct definitions. Here only used to make version number available to
# the source code. Include "exampleConfig.h" (no .in suffix) in the source.
configure_file (
  "${PROJECT_SOURCE_DIR}/include/exampleConfig.h.in"
  "${PROJECT_BINARY_DIR}/exampleConfig.h")
# add the binary tree to the search path for include files
# so that we will find exampleConfig.h
include_directories("${PROJECT_BINARY_DIR}")

# Ask CMake to output a compile_commands.json file for use with things like Vim YCM.
set( CMAKE_EXPORT_COMPILE_COMMANDS 1 )

# ---------------------------------------------------------------------------------
#                    Others Cmakelist.txt
# ---------------------------------------------------------------------------------
add_subdirectory(src)
#add_subdirectory(src/server/Modules)
