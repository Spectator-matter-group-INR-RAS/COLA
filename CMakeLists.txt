#
# Copyright (c) 2024-2025 Alexandr Svetlichnyi, Savva Savenkov, Artemii Novikov

# Permission is hereby granted, free of charge, to any person obtaining a copy of this software and
# associated documentation files (the "Software"), to deal in the Software without restriction,
# including without limitation the rights to use, copy, modify, merge, publish, distribute,
# sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:

# The above copyright notice and this permission notice shall be included in all copies or
# substantial portions of the Software.

# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT
# NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
# NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
# DAMAGES OR OTHER* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.

cmake_minimum_required(VERSION 3.13)
project(
    COLA
    VERSION 0.3.3
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 17)

# Add internal dependency to build
add_subdirectory(tinyxml2)

add_library(COLA SHARED COLA.cc)

target_compile_features(COLA PUBLIC cxx_std_17)

target_include_directories(
    COLA PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}> $<INSTALL_INTERFACE:include>
)

target_link_libraries(COLA PRIVATE tinyxml2)

set(COLA_CMAKE_CXX_CLANG_TIDY
    ""
    CACHE STRING "variable will be passed to CMAKE_CXX_CLANG_TIDY of COLA target"
)

set_target_properties(
    COLA
    PROPERTIES PUBLIC_HEADER "COLA.hh;LorentzVector.hh"
               CXX_CLANG_TIDY "${COLA_CMAKE_CXX_CLANG_TIDY}"
               VERSION "${COLA_VERSION}"
               SOVERSION "${COLA_VERSION_MAJOR}"
               CXX_CLANG_TIDY "${COLA_CMAKE_CXX_CLANG_TIDY}"
)

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

install(
    TARGETS COLA
    EXPORT COLAExport
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    INCLUDES
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

configure_package_config_file(
    "${PROJECT_SOURCE_DIR}/data/COLAConfig.cmake.in" "${CMAKE_CURRENT_BINARY_DIR}/COLAConfig.cmake"
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/COLA
)

write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/COLAConfigVersion.cmake COMPATIBILITY AnyNewerVersion
)

install(EXPORT COLAExport DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/COLA)

install(FILES "${CMAKE_CURRENT_BINARY_DIR}/COLAConfig.cmake"
              "${CMAKE_CURRENT_BINARY_DIR}/COLAConfigVersion.cmake"
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/COLA
)

install(FILES "${PROJECT_SOURCE_DIR}/data/config.sh"
        DESTINATION ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_BINDIR}
)

install(
    DIRECTORY cmake/
    DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/COLA/cmake
    FILES_MATCHING
    PATTERN "*.cmake"
)

enable_testing()
if (BUILD_TESTING)
    add_subdirectory(tests)
endif ()

include(cmake/COLAQualityTools.cmake)
setup_cola_quality_tools(EXCLUDE_PATTERNS ".*/tinyxml2/.*")
