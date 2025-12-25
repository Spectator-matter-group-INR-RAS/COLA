function (setup_cola_quality_tools)
    cmake_parse_arguments(QUALITY "" "" "EXCLUDE_PATTERNS" ${ARGN})

    set(DEFAULT_EXCLUDE_PATTERNS ".*/build/.*" ".*/third_party/.*" ".*/external/.*")
    set(EXCLUDE_PATTERNS ${DEFAULT_EXCLUDE_PATTERNS} ${QUALITY_EXCLUDE_PATTERNS})

    find_program(
        CLANG_FORMAT_EXE NAMES clang-format-21 clang-format-20 clang-format-19 clang-format
    )
    find_program(CLANG_TIDY_EXE NAMES clang-tidy-21 clang-tidy-20 clang-tidy-19 clang-tidy)
    find_program(CMAKE_FORMAT_EXE NAMES cmake-format)

    file(
        GLOB_RECURSE
        CPP_SOURCES
        "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/*.cc"
        "${CMAKE_CURRENT_SOURCE_DIR}/*.cxx"
        "${CMAKE_CURRENT_SOURCE_DIR}/*.hpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/*.hh"
        "${CMAKE_CURRENT_SOURCE_DIR}/*.hxx"
        "${CMAKE_CURRENT_SOURCE_DIR}/*.h"
    )

    file(GLOB_RECURSE CMAKE_FILES "${CMAKE_CURRENT_SOURCE_DIR}/*.cmake"
         "${CMAKE_CURRENT_SOURCE_DIR}/CMakeLists.txt"
    )

    foreach (PATTERN ${EXCLUDE_PATTERNS})
        list(FILTER CPP_SOURCES EXCLUDE REGEX "${PATTERN}")
        list(FILTER CMAKE_FILES EXCLUDE REGEX "${PATTERN}")
    endforeach ()

    if (CLANG_FORMAT_EXE)
        add_custom_target(
            clang-format-check
            COMMAND ${CLANG_FORMAT_EXE} --dry-run --Werror ${CPP_SOURCES}
            COMMENT "Check C++ formatting"
        )

        add_custom_target(
            clang-format-fix
            COMMAND ${CLANG_FORMAT_EXE} -i ${CPP_SOURCES}
            COMMENT "Fix C++ formatting"
        )
        message(STATUS "clang-format: ${CLANG_FORMAT_EXE}")
    endif ()

    if (CLANG_TIDY_EXE)
        add_custom_target(
            clang-tidy-check
            COMMAND ${CLANG_TIDY_EXE} -p ${CMAKE_BINARY_DIR} ${CPP_SOURCES}
            COMMENT "Run clang-tidy checks"
        )

        add_custom_target(
            clang-tidy-fix
            COMMAND ${CLANG_TIDY_EXE} -p ${CMAKE_BINARY_DIR} -fix ${CPP_SOURCES}
            COMMENT "Run clang-tidy with fixes"
        )
        message(STATUS "clang-tidy: ${CLANG_TIDY_EXE}")
    endif ()

    # CMake-format targets
    if (CMAKE_FORMAT_EXE)
        add_custom_target(
            cmake-format-check
            COMMAND ${CMAKE_FORMAT_EXE} --check ${CMAKE_FILES}
            COMMENT "Check CMake formatting"
        )

        add_custom_target(
            cmake-format-fix
            COMMAND ${CMAKE_FORMAT_EXE} -i ${CMAKE_FILES}
            COMMENT "Fix CMake formatting"
        )
        message(STATUS "cmake-format: ${CMAKE_FORMAT_EXE}")
    endif ()

    add_custom_target(
        uninstall
        COMMAND
            bash -c "
            if [ -f ${CMAKE_BINARY_DIR}/install_manifest.txt ]; then
                cat ${CMAKE_BINARY_DIR}/install_manifest.txt | xargs rm -f
                echo 'Uninstall completed'
            else
                echo 'No install_manifest.txt found'
            fi
        "
        COMMENT "Uninstalling project"
    )

endfunction ()
