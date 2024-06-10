cmake_minimum_required(VERSION 3.9)

set(USER_HOME $ENV{HOME})
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/../dll)
set(CMAKE_OSX_DEPLOYMENT_TARGET "11.0")

project(
        Project.Nar.SDL
        VERSION 100
        DESCRIPTION "Nar.SDL"
        LANGUAGES C
)

add_library(Nar.SDL SHARED
        include/Nar.SDL.h
        _package.c
        _package.h
        SDL.c
        MessageBox.c
        Video.c
)

find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})

target_include_directories(Nar.SDL PRIVATE ~/.nar/include)
set_target_properties(Nar.SDL PROPERTIES VERSION ${PROJECT_VERSION})

target_link_libraries(Nar.SDL PRIVATE ${SDL2_LIBRARIES})

file(COPY include DESTINATION ${USER_HOME}/.nar)