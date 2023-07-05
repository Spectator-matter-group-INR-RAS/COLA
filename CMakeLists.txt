cmake_minimum_required(VERSION 3.13)
project(COLA VERSION 0.2)

set(CMAKE_CXX_STANDARD 11)

add_library(COLAlib INTERFACE)
target_include_directories(COLAlib INTERFACE
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>)

set_target_properties(COLAlib PROPERTIES PUBLIC_HEADER "include/COLA.hh")

install(TARGETS COLAlib
        EXPORT COLAlibExport
        PUBLIC_HEADER DESTINATION include
        INCLUDES DESTINATION include) #Needed for proper EXPORT generation

include(CMakePackageConfigHelpers)

configure_package_config_file(
        "${PROJECT_SOURCE_DIR}/data/COLAlibConfig.cmake.in"
        "${CMAKE_CURRENT_BINARY_DIR}/COLAlibConfig.cmake"
        INSTALL_DESTINATION cmake)

write_basic_package_version_file(
        ${CMAKE_CURRENT_BINARY_DIR}/COLAlibConfigVersion.cmake
        COMPATIBILITY AnyNewerVersion)

install(EXPORT COLAlibExport
        DESTINATION cmake)

install(FILES "${CMAKE_CURRENT_BINARY_DIR}/COLAlibConfig.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/COLAlibConfigVersion.cmake"
        DESTINATION cmake)

install(FILES "${PROJECT_SOURCE_DIR}/data/config.sh"
        DESTINATION ${CMAKE_INSTALL_PREFIX})