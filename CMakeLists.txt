cmake_minimum_required(VERSION 2.6)
project(gdcmd)

set(GDCMD_VERSION_STRING 0.1-alpha)

configure_file(
 "${PROJECT_SOURCE_DIR}/config.h.in"
 "${PROJECT_SOURCE_DIR}/config.h"
)

include_directories("${PROJECT_SOURCE_DIR}")

set(SOURCE_FILES main.c gif.c)
add_executable(gdcmd ${SOURCE_FILES})

find_package(PkgConfig)
pkg_search_module(GD REQUIRED gdlib)
include_directories(${GD_INCLUDE_DIRS})
link_directories(${GD_LIBRARY_DIRS})
target_link_libraries(gdcmd ${GD_LIBRARIES})
