find_package(Qt6Core REQUIRED)

# get absolute path to qmake, then use it to find windeployqt executable

get_target_property(_qmake_executable Qt6::qmake IMPORTED_LOCATION)
get_filename_component(_qt_bin_dir "${_qmake_executable}" DIRECTORY)

function(windeployqt target)
    set(DEPLOY_DIR ../stage/${CMAKE_BUILD_TYPE})
    set(EXE_BIN ${PROJECT_NAME}.exe)
    set(STD_DLLS
            libstdc++-6.dll
            libgcc_s_seh-1.dll
            libwinpthread-1.dll
            )

    foreach (DLLNAME ${STD_DLLS})
        add_custom_command(TARGET ${target} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy
                ${_qt_bin_dir}/${DLLNAME}
                ${DEPLOY_DIR}/${DLLNAME}
                )
    endforeach ()

    string(TOLOWER ${CMAKE_BUILD_TYPE} BUILDTYPE)

#    add_custom_command(TARGET ${target} POST_BUILD
#            COMMAND ${CMAKE_COMMAND} -E copy
#            ${CMAKE_CURRENT_BINARY_DIR}/${EXE_BIN}
#            ${DEPLOY_DIR}/${EXE_BIN}
#            )
    add_custom_command(TARGET ${target} POST_BUILD
            COMMAND "${_qt_bin_dir}/windeployqt.exe"
            --verbose 1
            --${BUILDTYPE}
            --no-svg
            --no-opengl
            --no-opengl-sw
            --no-compiler-runtime
            --no-system-d3d-compiler
            --dir "${DEPLOY_DIR}"
            \"$<TARGET_FILE:${target}>\"
            COMMENT "Deploying Qt libraries using windeployqt for compilation target '${target}' ..."
            )

endfunction()