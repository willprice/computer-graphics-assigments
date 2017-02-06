cmake_minimum_required(VERSION 2.8 FATAL_ERROR)
project(raytracer CXX)
list(APPEND CMAKE_CXX_FLAGS "-O3")


set(CMAKE_MODULE_PATH
    ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

enable_testing()


add_subdirectory(src/cg)
add_subdirectory(src/raytracer)
