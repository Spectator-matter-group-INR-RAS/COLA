cmake_minimum_required(VERSION 3.22)
project(COLATest)

set(CMAKE_CXX_STANDARD 20)

set(CMAKE_CXX_FLAGS "-Wall -Wextra -Wpedantic -Werror")

# COLA lib
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/.. ${CMAKE_BINARY_DIR}/COLA)

enable_testing()

find_package(GTest REQUIRED)
include(GoogleTest)

set(Tests
    lorentz.cpp
)

add_executable(COLATest ${Tests})

target_link_libraries(COLATest COLA)
target_link_libraries(COLATest GTest::GTest GTest::Main)

gtest_discover_tests(COLATest)
