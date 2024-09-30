cmake_minimum_required(VERSION 3.13)
project(COLA VERSION 0.3.2)

set(CMAKE_CXX_STANDARD 17)

# Add internal dependency to build
add_subdirectory(tinyxml2)

add_library(COLA SHARED COLA.cc)

target_include_directories(COLA PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>)

target_link_libraries(COLA PRIVATE tinyxml2)

set_target_properties(COLA PROPERTIES
        PUBLIC_HEADER "COLA.hh"
        VERSION "${COLA_VERSION}"
        SOVERSION "${COLA_VERSION_MAJOR}")

install(TARGETS COLA
        EXPORT COLAExport
        LIBRARY DESTINATION lib
        PUBLIC_HEADER DESTINATION include
        INCLUDES DESTINATION include)
include(CMakePackageConfigHelpers)

configure_package_config_file(
        "${PROJECT_SOURCE_DIR}/data/COLAConfig.cmake.in"
        "${CMAKE_CURRENT_BINARY_DIR}/COLAConfig.cmake"
        INSTALL_DESTINATION lib/cmake/COLA)

write_basic_package_version_file(
        ${CMAKE_CURRENT_BINARY_DIR}/COLAConfigVersion.cmake
        COMPATIBILITY AnyNewerVersion)

install(EXPORT COLAExport
        DESTINATION lib/cmake/COLA)

install(FILES "${CMAKE_CURRENT_BINARY_DIR}/COLAConfig.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/COLAConfigVersion.cmake"
        DESTINATION lib/cmake/COLA)

install(FILES "${PROJECT_SOURCE_DIR}/data/config.sh"
        DESTINATION ${CMAKE_INSTALL_PREFIX})
