#!/usr/bin/env bash
set -x

BASE_DIR=/home/fe13/wp13824/linux
CMAKE="${BASE_DIR}/usr/bin/cmake"
SDL_LIBRARY="${BASE_DIR}/usr/lib/libSDL.so"
SDL_INCLUDE_DIR="${BASE_DIR}/usr/include/SDL"
GLM_INCLUDE_DIR="${BASE_DIR}/usr/include/"

mkdir -p build
cd build
"${CMAKE}" -DGLM_INCLUDE_DIR="$GLM_INCLUDE_DIR" \
    -DSDL_INCLUDE_DIR="$SDL_INCLUDE_DIR" \
    -DSDL_LIBRARY="$SDL_LIBRARY" \
    ..
make
