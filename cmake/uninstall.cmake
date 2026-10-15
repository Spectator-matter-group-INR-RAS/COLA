if (NOT DEFINED COLA_INSTALL_MANIFEST)
    set(COLA_INSTALL_MANIFEST "${CMAKE_BINARY_DIR}/install_manifest.txt")
endif ()

if (NOT EXISTS "${COLA_INSTALL_MANIFEST}")
    message(STATUS "No install_manifest.txt found: ${COLA_INSTALL_MANIFEST}")
    return()
endif ()

file(STRINGS "${COLA_INSTALL_MANIFEST}" installed_files)
foreach (installed_file IN LISTS installed_files)
    set(staged_file "$ENV{DESTDIR}${installed_file}")
    if (EXISTS "${staged_file}" OR IS_SYMLINK "${staged_file}")
        execute_process(
            COMMAND "${CMAKE_COMMAND}" -E remove "${staged_file}"
            RESULT_VARIABLE remove_result
            ERROR_VARIABLE remove_error
        )
        if (NOT remove_result EQUAL 0)
            message(FATAL_ERROR "Could not remove ${staged_file}: ${remove_error}")
        endif ()
        message(STATUS "Removed: ${staged_file}")
    endif ()
endforeach ()
message(STATUS "Uninstall completed")
