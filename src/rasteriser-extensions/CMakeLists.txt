cmake_minimum_required(VERSION 2.8 FATAL_ERROR)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH}
    ${CMAKE_CURRENT_SOURCE_DIR}/../../cmake)


project(rasteriser-extensions CXX C)
set(CMAKE_CXX_STANDARD 11)

add_subdirectory(src)
