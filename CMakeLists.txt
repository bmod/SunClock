cmake_minimum_required(VERSION 3.18)
project(sunclock VERSION 0.0.1 LANGUAGES CXX)

set(DEPLOY_DIR ${CMAKE_CACHEFILE_DIR})

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

if (MSVC)
    add_compile_options(/Zc:__cplusplus /permissive-)
endif ()

find_package(Qt5 REQUIRED COMPONENTS Core Widgets Concurrent)
get_target_property(_qmake_executable Qt5::qmake IMPORTED_LOCATION)
get_filename_component(_qt_bin_dir "${_qmake_executable}" DIRECTORY)

file(GLOB_RECURSE SOURCES src/*.cpp)
file(GLOB_RECURSE HEADERS src/*.h)
file(GLOB_RECURSE RESOURCES res/*.qrc)

add_executable(${PROJECT_NAME} ${SOURCES} ${RESOURCES})

target_include_directories(${PROJECT_NAME} PRIVATE src)

set(QT_LIBS Qt5::Core Qt5::Widgets Qt5::Concurrent)
target_link_libraries(${PROJECT_NAME} ${QT_LIBS})

configure_file(config.json ${CMAKE_CURRENT_BINARY_DIR})

