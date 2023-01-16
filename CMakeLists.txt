cmake_minimum_required(VERSION 3.24)
project(GeneratorFramework)

set(CMAKE_CXX_STANDARD 11)

include_directories(${PROJECT_SOURCE_DIR}/Converters)
include_directories(${PROJECT_SOURCE_DIR}/Generators)
include_directories(${PROJECT_SOURCE_DIR}/Writers)
include_directories(${PROJECT_SOURCE_DIR}/Factories)
include_directories(${PROJECT_SOURCE_DIR}/Management)

file(GLOB sources ${PROJECT_SOURCE_DIR}/Converters/*.cc ${PROJECT_SOURCE_DIR}/Generators/*.cc ${PROJECT_SOURCE_DIR}/Writers/*.cc ${PROJECT_SOURCE_DIR}/Management/*.cc)
file(GLOB headers ${PROJECT_SOURCE_DIR}/Converters/*.hh ${PROJECT_SOURCE_DIR}/Generators/*.hh ${PROJECT_SOURCE_DIR}/Writers/*.hh ${PROJECT_SOURCE_DIR}/Management/*.hh ${PROJECT_SOURCE_DIR}/Factories)

add_executable(GeneratorFramework main.cc ${sources} ${headers})