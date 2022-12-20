cmake_minimum_required(VERSION 3.24)
project(GeneratorFramework)
set(CMAKE_CXX_STANDARD 11)

file(GLOB sources ${PROJECT_SOURCE_DIR}/Converters ${PROJECT_SOURCE_DIR}/Generators ${PROJECT_SOURCE_DIR}/Writers ${PROJECT_SOURCE_DIR}/Management)
file(GLOB headers ${PROJECT_SOURCE_DIR}/Converters ${PROJECT_SOURCE_DIR}/Generators ${PROJECT_SOURCE_DIR}/Writers ${PROJECT_SOURCE_DIR}/Management)

add_executable(GeneratorFramework main.cc ${sources} ${headers})
