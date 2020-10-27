# CMakeList.txt : CMake project

set(BINARY ${CMAKE_PROJECT_NAME}_test)

############################
# Configure GoogleTest build

configure_file(
    ${CMAKE_SOURCE_DIR}/deps/gtest/CMakeLists.txt.in
    ${CMAKE_SOURCE_DIR}/build/googletest-download/CMakeLists.txt
)
execute_process(
	COMMAND ${CMAKE_COMMAND} -G ${CMAKE_GENERATOR} .
	RESULT_VARIABLE result
	WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/build/googletest-download
)
if(result)
	message(FATAL_ERROR "CMake step for googletest failed: ${result}")
endif()

##################
# Build GoogleTest

execute_process(
	COMMAND ${CMAKE_COMMAND} --build .
	RESULT_VARIABLE result
	WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/build/googletest-download
)
if(result)
  message(FATAL_ERROR "Build step for googletest failed: ${result}")
endif()

# Prevent overriding the parent project's compiler/linker
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
add_subdirectory(
	${CMAKE_SOURCE_DIR}/build/googletest-src
	${CMAKE_SOURCE_DIR}/build/googletest-build
	EXCLUDE_FROM_ALL
)
##############################
# organize include directories

set(
	GTEST_INCLUDE_DIR
	${CMAKE_SOURCE_DIR}/build/googletest-src/googlemock/include/
	${CMAKE_SOURCE_DIR}/build/googletest-src/googletest/include/
)
set(
	TEST_INCLUDE_DIR
	${CMAKE_CURRENT_SOURCE_DIR}/include/
)
file(
	GLOB_RECURSE
	TEST_SRC_FILES
	${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
)
include_directories(
	${GTEST_INCLUDE_DIR}
	${TEST_INCLUDE_DIR}
	${TOOLSET_INCLUDE_DIR}
)
SET(COVERAGE OFF CACHE BOOL "Coverage")
add_executable (${BINARY} ${TEST_SRC_FILES})
if (COVERAGE)
    target_compile_options(${BINARY} PRIVATE --coverage )
    target_link_libraries(${BINARY} PRIVATE ${CMAKE_PROJECT_NAME} gmock_main --coverage)
else()
	target_link_libraries(${BINARY} ${CMAKE_PROJECT_NAME} gmock_main)
endif()