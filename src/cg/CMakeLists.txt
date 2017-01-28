cmake_minimum_required(VERSION 2.8 FATAL_ERROR)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH}
    ${CMAKE_CURRENT_SOURCE_DIR}/../../cmake)
set(CMAKE_CXX_STANDARD 11)

include(FindGLM)

set(VERSION_MAJOR "0")
set(VERSION_MINOR "0")
set(VERSION_PATCH "0")
set(VERSION ${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH})

set(LIBRARY_NAME CG)
project(${LIBRARY_NAME} LANGUAGES CXX)

find_package(SDL REQUIRED)
find_package(GLM REQUIRED)

include_directories(
    include
    ${GLM_INCLUDE_DIR}
    ${SDL_INCLUDE_DIR}
)

add_subdirectory(src)
add_subdirectory(test)

set(${PROJECT_NAME}_INCLUDE_DIR
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    CACHE INTERNAL
    "${PROJECT_NAME}: Include directories")
