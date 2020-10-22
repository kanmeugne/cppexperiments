# CMakeList.txt : CMake project
cmake_minimum_required (VERSION 3.8)

set(BINARY ${CMAKE_PROJECT_NAME})

##############################
# organize include directories

set(APP_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include/)
file(GLOB_RECURSE APP_SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)
include_directories(${APP_INCLUDE_DIR})

add_executable (${BINARY} ${APP_SRC_FILES})
add_library(${BINARY}_lib STATIC ${SOURCES})

source_group("src" FILES ${APP_SRC_FILES})
source_group("include" FILES ${APP_INCLUDE_DIR}/*.h)