cmake_minimum_required(VERSION 2.8 FATAL_ERROR)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH}
    ${CMAKE_CURRENT_SOURCE_DIR}/../../cmake)
include(FindGLM)

find_package(SDL REQUIRED)
find_package(GLM REQUIRED)
find_package(OpenMP REQUIRED)

include_directories(
    ${SDL_INCLUDE_DIR}
    ${GLM_INCLUDE_DIR}
    ${CG_INCLUDE_DIR}
)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")

file(GLOB_RECURSE SRC_FILES 
    RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    *.cpp
)

set(RAYTRACER_NAME raytracer-extensions)
add_executable(${RAYTRACER_NAME} ${SRC_FILES})

target_link_libraries(${RAYTRACER_NAME} CG ${SDL_LIBRARY} ${GLM_LIBRARY})
