cmake_minimum_required(VERSION 3.24)
project(GeneratorFramework)

set(CMAKE_CXX_STANDARD 11)

include_directories(${PROJECT_SOURCE_DIR}/Converters)
include_directories(${PROJECT_SOURCE_DIR}/Generators)
include_directories(${PROJECT_SOURCE_DIR}/Writers)
include_directories(${PROJECT_SOURCE_DIR}/Factories)
include_directories(${PROJECT_SOURCE_DIR}/Management)

file(GLOB sources ${PROJECT_SOURCE_DIR}/Converters ${PROJECT_SOURCE_DIR}/Generators ${PROJECT_SOURCE_DIR}/Writers ${PROJECT_SOURCE_DIR}/Management ${PROJECT_SOURCE_DIR}/Factories)
file(GLOB headers ${PROJECT_SOURCE_DIR}/Converters ${PROJECT_SOURCE_DIR}/Generators ${PROJECT_SOURCE_DIR}/Writers ${PROJECT_SOURCE_DIR}/Management ${PROJECT_SOURCE_DIR}/Factories)

add_executable(GeneratorFramework main.cc ${sources} ${headers})