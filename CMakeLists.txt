cmake_minimum_required(VERSION 3.16)
set(CMAKE_CXX_STANDARD 17)
project(sunclock LANGUAGES CXX)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
option(BUILD_SHARED_LIBS "Build shared libraries" OFF)

include(FetchContent)

FetchContent_Declare(SFML
        GIT_REPOSITORY https://github.com/SFML/SFML.git
        GIT_TAG 2.6.x)
FetchContent_MakeAvailable(SFML)

FetchContent_Declare(nlohmann_json
        GIT_REPOSITORY https://github.com/nlohmann/json.git
        GIT_TAG v3.11.3)
FetchContent_MakeAvailable(nlohmann_json)

FetchContent_Declare(dategit
        GIT_REPOSITORY https://github.com/HowardHinnant/date.git
        GIT_TAG v3.0.1)
FetchContent_MakeAvailable(dategit)


file(GLOB_RECURSE SOURCES src/*.cpp)
file(GLOB_RECURSE HEADERS src/*.h)
file(GLOB_RECURSE RESOURCES res/*)

# NOTE: This just happened: https://github.com/HowardHinnant/date/issues/836
# I don't want to spend time on it, so I'm just going to use the OS timezone Db, may not work in Windows
add_compile_definitions(USE_OS_TZDB=1)
# (old attempt for posterity)
# add_compile_definitions(AUTO_DOWNLOAD=0) # Keep date.git from downloading an up-to-date timezone file

add_executable(${PROJECT_NAME} ${SOURCES} ${dategit_SOURCE_DIR}/src/tz.cpp)

target_link_libraries(${PROJECT_NAME} PRIVATE sfml-graphics curl)
target_include_directories(${PROJECT_NAME} PUBLIC ${nlohmann_json_SOURCE_DIR}/include)
target_include_directories(${PROJECT_NAME} PUBLIC ${dategit_SOURCE_DIR}/include)

foreach (RES ${RESOURCES})
    string(REPLACE ${CMAKE_CURRENT_LIST_DIR}/res "" RES_REL ${RES})

    configure_file(
            ${RES}
            ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/res/${RES_REL} COPYONLY
    )
endforeach ()

if (WIN32)
    add_custom_command(
            TARGET ${PROJECT_NAME}
            COMMENT "Copy OpenAL DLL"
            PRE_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${SFML_SOURCE_DIR}/extlibs/bin/$<IF:$<EQUAL:${CMAKE_SIZEOF_VOID_P},8>,x64,x86>/openal32.dll $<TARGET_FILE_DIR:main>
            VERBATIM)
endif ()