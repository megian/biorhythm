cmake_minimum_required(VERSION 3.5)

project(biorhythmus)

set(BIORHYTHMUS_MAJOR_VERSION 0)
set(BIORHYTHMUS_MINOR_VERSION 1)
set(BIORHYTHMUS_PATCH_VERSION 0)
set(VERSION
  ${BIORHYTHMUS_MAJOR_VERSION}.${FOOBAR_MINOR_VERSION}.${FOOBAR_PATCH_VERSION})
set(CMAKE_C_CLANG_TIDY clang-tidy -checks=-*,readability-*)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/src/config.h.in "${CMAKE_CURRENT_BINARY_DIR}/src/config.h" @ONLY)

find_program(INTLTOOL_MERGE intltool-merge)
if(${INTLTOOL_MERGE} STREQUAL "INTLTOOL_MERGE-NOTFOUND")
  message(FATAL_ERROR "intltool-merge not found, you can find one in http://ftp.gnome.org/pub/gnome/sources/intltool/")
endif()

set (GETTEXT_PACKAGE "biorhythmus")
add_definitions(-DGETTEXT_PACKAGE=\"${GETTEXT_PACKAGE}\")

add_subdirectory (data)
add_subdirectory (src)
add_subdirectory (po)
