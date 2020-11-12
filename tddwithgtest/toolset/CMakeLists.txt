cmake_minimum_required (VERSION 3.8)

set(BINARY ${CMAKE_PROJECT_NAME})

######################################
# organize include and src directories

set(TOOLSET_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include/)
set(TOOLSET_INCLUDE_DIR ${TOOLSET_INCLUDE_DIR} PARENT_SCOPE)
file(GLOB_RECURSE TOOLSET_SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)
include_directories(${TOOLSET_INCLUDE_DIR})

add_library(${BINARY} ${TOOLSET_SRC_FILES})