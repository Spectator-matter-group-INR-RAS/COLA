cmake_minimum_required(VERSION 3.13)
project(COLA VERSION 0.2)

set(CMAKE_CXX_STANDARD 11)

add_library(COLAlib INTERFACE)
target_include_directories(COLAlib INTERFACE
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/api>
        $<INSTALL_INTERFACE:api>)

set_target_properties(COLAlib PROPERTIES PUBLIC_HEADER "api/COLA.hh")

if(NOT INSTALL_DIR)
    set(INSTALL_DIR ${PROJECT_SOURCE_DIR}/install)
endif()

install(TARGETS COLAlib
        EXPORT COLA
        PUBLIC_HEADER DESTINATION ${INSTALL_DIR}/api)

include(CMakePackageConfigHelpers)

configure_package_config_file(
        "${PROJECT_SOURCE_DIR}/data/COLAConfig.cmake.in"
        "${CMAKE_CURRENT_BINARY_DIR}/COLAConfig.cmake"
        INSTALL_DESTINATION ${INSTALL_DIR}/cmake)

write_basic_package_version_file(
        ${CMAKE_CURRENT_BINARY_DIR}/COLAConfigVersion.cmake
        COMPATIBILITY AnyNewerVersion)

install(EXPORT COLA
        DESTINATION ${INSTALL_DIR}/cmake)

install(FILES "${CMAKE_CURRENT_BINARY_DIR}/COLAConfig.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/COLAConfigVersion.cmake"
        DESTINATION ${INSTALL_DIR}/cmake)

install(FILES "${PROJECT_SOURCE_DIR}/data/config.sh"
        DESTINATION ${INSTALL_DIR})