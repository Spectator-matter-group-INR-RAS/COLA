if (NOT SETUP_QUALITY_SOURCE_DIR)
    set(SETUP_QUALITY_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR})
endif ()

if (NOT SETUP_QUALITY_BINARY_DIR)
    set(SETUP_QUALITY_BINARY_DIR ${CMAKE_BINARY_DIR})
endif ()

# Default exclude patterns
set(DEFAULT_EXCLUDE_PATTERNS ".*/build/.*" ".*/tinyxml2/.*")

# Merge default and user-provided exclude patterns
set(EXCLUDE_PATTERNS ${DEFAULT_EXCLUDE_PATTERNS} ${SETUP_QUALITY_EXCLUDE_PATTERNS})

# Find clang-format
find_program(
    CLANG_FORMAT_EXE NAMES clang-format-19 clang-format-18 clang-format-17 clang-format-16
                           clang-format-15 clang-format
)

# Find cmake-format
find_program(CMAKE_FORMAT_EXE NAMES cmake-format)

# Get all C++ source files
file(
    GLOB_RECURSE
    ALL_SOURCES
    "${SETUP_QUALITY_SOURCE_DIR}/*.cc"
    "${SETUP_QUALITY_SOURCE_DIR}/*.cpp"
    "${SETUP_QUALITY_SOURCE_DIR}/*.cxx"
    "${SETUP_QUALITY_SOURCE_DIR}/*.hh"
    "${SETUP_QUALITY_SOURCE_DIR}/*.hpp"
    "${SETUP_QUALITY_SOURCE_DIR}/*.hxx"
    "${SETUP_QUALITY_SOURCE_DIR}/*.h"
)

# Apply exclude patterns
foreach (PATTERN ${EXCLUDE_PATTERNS})
    list(FILTER ALL_SOURCES EXCLUDE REGEX "${PATTERN}")
endforeach ()

# Setup clang-format targets
if (CLANG_FORMAT_EXE)
    add_custom_target(
        clang-format-check
        COMMAND ${CLANG_FORMAT_EXE} --dry-run --Werror ${ALL_SOURCES}
        WORKING_DIRECTORY ${SETUP_QUALITY_SOURCE_DIR}
        COMMENT "Checking code formatting with clang-format"
    )

    add_custom_target(
        clang-format-fix
        COMMAND ${CLANG_FORMAT_EXE} -i ${ALL_SOURCES}
        WORKING_DIRECTORY ${SETUP_QUALITY_SOURCE_DIR}
        COMMENT "Fixing code formatting with clang-format"
    )

    message(STATUS "clang-format found: ${CLANG_FORMAT_EXE}")
    message(STATUS "  - Use 'cmake --build . --target clang-format-check' to check formatting")
    message(STATUS "  - Use 'cmake --build . --target clang-format-fix' to fix formatting")
else ()
    message(WARNING "clang-format not found. clang-format targets will not be available.")
endif ()

# Get all CMake files
file(GLOB_RECURSE CMAKE_FILES "${SETUP_QUALITY_SOURCE_DIR}/*.cmake"
     "${SETUP_QUALITY_SOURCE_DIR}/CMakeLists.txt"
)

# Apply exclude patterns
foreach (PATTERN ${EXCLUDE_PATTERNS})
    list(FILTER CMAKE_FILES EXCLUDE REGEX "${PATTERN}")
endforeach ()

# Setup cmake-format targets
if (CMAKE_FORMAT_EXE)
    add_custom_target(
        cmake-format-check
        COMMAND ${CMAKE_FORMAT_EXE} --check ${CMAKE_FILES}
        WORKING_DIRECTORY ${SETUP_QUALITY_SOURCE_DIR}
        COMMENT "Checking CMake file formatting with cmake-format"
    )

    add_custom_target(
        cmake-format-fix
        COMMAND ${CMAKE_FORMAT_EXE} -i ${CMAKE_FILES}
        WORKING_DIRECTORY ${SETUP_QUALITY_SOURCE_DIR}
        COMMENT "Fixing CMake file formatting with cmake-format"
    )

    message(STATUS "cmake-format found: ${CMAKE_FORMAT_EXE}")
    message(
        STATUS "  - Use 'cmake --build . --target cmake-format-check' to check CMake formatting"
    )
    message(STATUS "  - Use 'cmake --build . --target cmake-format-fix' to fix CMake formatting")
else ()
    message(WARNING "cmake-format not found. cmake-format targets will not be available.")
endif ()
