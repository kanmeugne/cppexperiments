# CMakeList.txt : CMake project
cmake_minimum_required (VERSION 3.8)

set(BINARY ${CMAKE_PROJECT_NAME})

##############################
# organize include directories

set(TOOLSET_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include/)
file(GLOB_RECURSE TOOLSET_SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)
include_directories(${TOOLSET_INCLUDE_DIR})

add_library(${BINARY} STATIC ${TOOLSET_SRC_FILES})

source_group("src" FILES ${TOOLSET_SRC_FILES})
source_group("include" FILES ${TOOLSET_INCLUDE_DIR}/*.h)