cmake_minimum_required(VERSION 3.16)
set(CMAKE_CXX_STANDARD 17)
project(sunclock LANGUAGES CXX)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
option(BUILD_SHARED_LIBS "Build shared libraries" OFF)

include(FetchContent)

file(GLOB_RECURSE SOURCES src/*.cpp)
file(GLOB_RECURSE HEADERS src/*.h)
file(GLOB_RECURSE RESOURCES res/*)

# NOTE: This just happened: https://github.com/HowardHinnant/date/issues/836
# I don't want to spend time on it, so I'm just going to use the OS timezone Db, may not work in Windows
add_compile_definitions(USE_OS_TZDB=1)
# (old attempt for posterity)
# add_compile_definitions(AUTO_DOWNLOAD=0) # Keep date.git from downloading an up-to-date timezone file

set(DEPLOY_DIR ${CMAKE_CACHEFILE_DIR})

project(PiClock)

set(_qt_bin_dir ${Qt5_DIR}/../../../bin)
#get_target_property(_qmake_executable Qt5::qmake IMPORTED_LOCATION)
#get_filename_component(_qt_bin_dir "${_qmake_executable}" DIRECTORY)
#include(raspberrypi-tools.cmake)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)


find_package(Qt5 COMPONENTS
        Core
        Gui
        Widgets
        OpenGL
        REQUIRED)

file(GLOB_RECURSE SOURCES src/*.cpp)
file(GLOB_RECURSE HEADERS src/*.h)
file(GLOB_RECURSE RESOURCES res/*.qrc)
add_executable(${PROJECT_NAME} ${SOURCES} ${RESOURCES})
target_include_directories(${PROJECT_NAME} PRIVATE src)
target_link_libraries(${PROJECT_NAME}
        Qt::Core
        Qt::Gui
        Qt::Widgets
        Qt::OpenGL
)



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

# ====== QT
if (WIN32)
    set(EXE_BIN ${PROJECT_NAME}.exe)
    string(TOLOWER ${CMAKE_BUILD_TYPE} CMAKE_BUILD_TYPE_LOWER)

    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND "${_qt_bin_dir}/windeployqt.exe"
            --verbose 1
            --no-svg
            --no-compiler-runtime
            --dir "${DEPLOY_DIR}"
            \"$<TARGET_FILE:${PROJECT_NAME}>\"
            COMMENT "Deploying Qt libraries using windeployqt for compilation target '${PROJECT_NAME}' (${CMAKE_BUILD_TYPE_LOWER}) in ${DEPLOY_DIR}"
    )
endif ()
#install(TARGETS ${PROJECT_NAME} DESTINATION bin)
#install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION bin COMPONENT Runtime)


