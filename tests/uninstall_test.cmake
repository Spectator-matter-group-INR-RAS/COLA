cmake_minimum_required(VERSION 3.13)

function (run)
    execute_process(
        COMMAND ${ARGV}
        RESULT_VARIABLE result
        OUTPUT_VARIABLE output
        ERROR_VARIABLE error
    )
    if (NOT result EQUAL 0)
        message(FATAL_ERROR "Command failed: ${ARGV}\n${output}\n${error}")
    endif ()
endfunction ()

file(REMOVE_RECURSE "${TEST_ROOT}")
file(MAKE_DIRECTORY "${TEST_ROOT}/sdk/cmake" "${TEST_ROOT}/consumer")
# The consumer has no cmake/uninstall.cmake: the installed module must find its own sibling.
file(COPY "${COLA_SOURCE_DIR}/cmake/COLAQualityTools.cmake"
          "${COLA_SOURCE_DIR}/cmake/uninstall.cmake" DESTINATION "${TEST_ROOT}/sdk/cmake"
)
file(WRITE "${TEST_ROOT}/consumer/payload with spaces.txt" "installed payload\n")
file(
    WRITE "${TEST_ROOT}/consumer/CMakeLists.txt"
    [=[
cmake_minimum_required(VERSION 3.13)
project(UninstallConsumer LANGUAGES NONE)
include("${QUALITY_MODULE}")
setup_cola_quality_tools()
install(FILES "${CMAKE_CURRENT_SOURCE_DIR}/payload with spaces.txt" DESTINATION share)
]=]
)

set(build_dir "${TEST_ROOT}/build")
set(prefix "${TEST_ROOT}/install prefix")
run("${CMAKE_COMMAND}"
    -S
    "${TEST_ROOT}/consumer"
    -B
    "${build_dir}"
    "-DCMAKE_INSTALL_PREFIX=${prefix}"
    "-DQUALITY_MODULE=${TEST_ROOT}/sdk/cmake/COLAQualityTools.cmake"
)

# No manifest is a successful no-op, as in the previous implementation.
run("${CMAKE_COMMAND}" --build "${build_dir}" --target uninstall)
run("${CMAKE_COMMAND}" --build "${build_dir}" --target install)
if (NOT EXISTS "${prefix}/share/payload with spaces.txt")
    message(FATAL_ERROR "Fixture was not installed")
endif ()
file(WRITE "${prefix}/keep.txt" "not owned by this installation\n")
run("${CMAKE_COMMAND}" --build "${build_dir}" --target uninstall)
if (EXISTS "${prefix}/share/payload with spaces.txt" OR NOT EXISTS "${prefix}/keep.txt")
    message(FATAL_ERROR "Uninstall did not respect the manifest")
endif ()
run("${CMAKE_COMMAND}" --build "${build_dir}" --target uninstall)

# DESTDIR must select staged files, leaving the unstaged installation alone.
file(WRITE "${prefix}/share/payload with spaces.txt" "unstaged sentinel\n")
set(stage "${TEST_ROOT}/stage")
run("${CMAKE_COMMAND}"
    -E
    env
    "DESTDIR=${stage}"
    "${CMAKE_COMMAND}"
    --build
    "${build_dir}"
    --target
    install
)
if (NOT EXISTS "${stage}${prefix}/share/payload with spaces.txt")
    message(FATAL_ERROR "Staged fixture was not installed")
endif ()
run("${CMAKE_COMMAND}"
    -E
    env
    "DESTDIR=${stage}"
    "${CMAKE_COMMAND}"
    --build
    "${build_dir}"
    --target
    uninstall
)
if (EXISTS "${stage}${prefix}/share/payload with spaces.txt"
    OR NOT EXISTS "${prefix}/share/payload with spaces.txt"
)
    message(FATAL_ERROR "Uninstall did not respect DESTDIR")
endif ()

if (UNIX)
    set(link "${prefix}/broken link")
    run("${CMAKE_COMMAND}" -E create_symlink "missing-target" "${link}")
    file(APPEND "${build_dir}/install_manifest.txt" "\n${link}\n")
    run("${CMAKE_COMMAND}" --build "${build_dir}" --target uninstall)
    if (IS_SYMLINK "${link}")
        message(FATAL_ERROR "Uninstall did not remove a dangling symlink")
    endif ()
endif ()
