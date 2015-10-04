
# Use the package PkgConfig to detect GTK+ headers/library files
find_package(PkgConfig REQUIRED)
pkg_check_modules(GLIB REQUIRED glib-2.0)
pkg_check_modules(GTK3 REQUIRED gtk+-3.0)
pkg_check_modules(JSON-GLIB REQUIRED json-glib-1.0)

find_library(M_LIB m)

# Setup CMake to use GTK+, tell the compiler where to look for headers
# and to the linker where to look for libraries
include_directories(
	${CMAKE_CURRENT_SOURCE_DIR}
	${GLIB2_INCLUDE_DIRS}
	${GTK3_INCLUDE_DIRS}
	${JSON-GLIB_INCLUDE_DIRS}
	)

set (GETTEXT_PACKAGE "biorhythmus")
add_definitions(-DGETTEXT_PACKAGE=\"${GETTEXT_PACKAGE}\")

link_directories(${GTK3_LIBRARY_DIRS})

# Add other flags to the compiler
add_definitions(${GTK3_CFLAGS_OTHER})

#However, the file(GLOB...) allows for wildcard additions:
file(GLOB SOURCES "*.c")
 
add_executable(biorhythmus ${SOURCES})

# Link the target to the GTK+ libraries
target_link_libraries(biorhythmus 
	${GTK3_LIBRARIES}
	${JSON-GLIB_LIBRARIES}
	${M_LIB}
	)
