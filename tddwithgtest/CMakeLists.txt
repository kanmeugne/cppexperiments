# CMakeList.txt : Upper level configuration file (here you can add subprojects and/or manage external depenencies)
# et incluez les sous-projets ici.
#
cmake_minimum_required (VERSION 3.10)

set(
    CMAKE_RUNTIME_OUTPUT_DIRECTORY
    ${CMAKE_SOURCE_DIR}/bin/${CMAKE_BUILD_TYPE}/
)
set(
    CMAKE_LIBRARY_OUTPUT_DIRECTORY
    ${CMAKE_SOURCE_DIR}/lib/${CMAKE_BUILD_TYPE}/
)
set(
    CMAKE_ARCHIVE_OUTPUT_DIRECTORY 
    ${CMAKE_SOURCE_DIR}/build/${CMAKE_GENERATOR_PLATFORM}/${CMAKE_BUILD_TYPE}/
)

project (toolset C CXX)

# Incluez les sous-projets
add_subdirectory ("toolset")
add_subdirectory("tests")