cmake_minimum_required(VERSION 2.6)
project(gdcmd)

set(GDCMD_VERSION 0.1-alpha)

configure_file(
 "${PROJECT_SOURCE_DIR}/config.h.in"
 "${PROJECT_SOURCE_DIR}/config.h"
)

include_directories("${PROJECT_SOURCE_DIR}")

set(SOURCE_FILES main.c)
add_executable(gdcmd ${SOURCE_FILES})