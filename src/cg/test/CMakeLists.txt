cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

file(GLOB_RECURSE TEST_FILES RELATIVE
    ${CMAKE_CURRENT_SOURCE_DIR}
    tests/*.cpp
)

set(TEST_RUNNER_NAME ${PROJECT_NAME}_tests)

add_executable(${TEST_RUNNER_NAME}
    test_runner.cpp
    ${TEST_FILES}
)
target_link_libraries(${TEST_RUNNER_NAME}
    ${PROJECT_NAME}
    ${SDL_LIBRARY}
)
add_test(NAME ${TEST_RUNNER_NAME} COMMAND ${TEST_RUNNER_NAME})
