cmake_minimum_required(VERSION 3.18)
project(sunclock VERSION 0.0.1 LANGUAGES CXX)

include(cmake/targetarch.cmake)

set(DEPLOY_DIR ${CMAKE_CACHEFILE_DIR})

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

if (MSVC)
    add_compile_options(/Zc:__cplusplus /permissive-)
endif ()

find_package(Qt5 REQUIRED COMPONENTS Core Widgets Concurrent OpenGL Multimedia MultimediaWidgets)
get_target_property(_qmake_executable Qt5::qmake IMPORTED_LOCATION)
get_filename_component(_qt_bin_dir "${_qmake_executable}" DIRECTORY)

file(GLOB_RECURSE SOURCES src/*.cpp)
file(GLOB_RECURSE HEADERS src/*.h)
file(GLOB_RECURSE RESOURCES res/*.qrc)


add_executable(${PROJECT_NAME} ${SOURCES} ${RESOURCES})

target_include_directories(${PROJECT_NAME} PRIVATE src)

set(QT_LIBS Qt5::Core Qt5::Widgets Qt5::Concurrent Qt5::OpenGL Qt5::Multimedia Qt5::MultimediaWidgets)
target_link_libraries(${PROJECT_NAME} ${QT_LIBS})

# Core libraries, for windows
if (WIN32)
    if (MSVC)
    else ()
        set(STD_DLLS
                libstdc++-6.dll
                libgcc_s_seh-1.dll
                libwinpthread-1.dll
        )
        foreach (DLLNAME ${STD_DLLS})
            configure_file(${_qt_bin_dir}/${DLLNAME}
                    ${DEPLOY_DIR}/${DLLNAME}
                    COPYONLY)
        endforeach ()
    endif ()
endif ()


# Config file
configure_file(config.json ${CMAKE_CURRENT_BINARY_DIR})

# QT
if (WIN32)
    set(EXE_BIN ${PROJECT_NAME}.exe)
    string(TOLOWER ${CMAKE_BUILD_TYPE} CMAKE_BUILD_TYPE_LOWER)

    #    configure_file(${CMAKE_CURRENT_BINARY_DIR}/${EXE_BIN} ${DEPLOY_DIR}/${EXE_BIN} COPYONLY)

    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND "${_qt_bin_dir}/windeployqt.exe"
            --verbose 1
            #            --debug
            --no-svg
            #        --no-opengl
            #        --no-opengl-sw
            --no-compiler-runtime
            #        --no-system-d3d-compiler
            --dir "${DEPLOY_DIR}"
            \"$<TARGET_FILE:${PROJECT_NAME}>\"
            COMMENT "Deploying Qt libraries using windeployqt for compilation target '${PROJECT_NAME}' (${CMAKE_BUILD_TYPE_LOWER}) in ${DEPLOY_DIR}"
    )
endif ()
