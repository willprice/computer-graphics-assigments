cmake_minimum_required(VERSION 2.8 FATAL_ERROR)
project(raytracer CXX)
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall -O3")
set(CMAKE_CXX_FLAGS_RELEASE  "${CMAKE_CXX_FLAGS_RELEASE} -Wall -O3")

set(CMAKE_MODULE_PATH
    ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

enable_testing()

add_subdirectory(src/cg)
add_subdirectory(src/raytracer)
add_subdirectory(src/rasteriser)
