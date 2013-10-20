#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT(Biorhythm, 0.15, gabisoft@freesurf.ch)
AM_INIT_AUTOMAKE
AC_CONFIG_HEADERS([config.h])

AC_PROG_CC
if test "$GCC" = "yes"; then
        CFLAGS="$CFLAGS -Wall"
fi
AM_PROG_CC_C_O

PKG_CHECK_MODULES(GTK, gtk+-3.0)

# check for json-glib
PKG_CHECK_MODULES(JSONGLIB, [json-glib-1.0], [
            AC_DEFINE(USE_JSONGLIB, 1, [Use JSONGLIB])
            AC_SUBST(JSONGLIB_CFLAGS)
            AC_SUBST(JSONGLIB_LIBS)
            ] ,
            )

# Checks for libraries.
AC_CHECK_LIB([m],[floor])

# Checks for header files.

# Checks for typedefs, structures, and compiler characteristics.

# Checks for library functions.
AC_CHECK_FUNCS([floor])

AM_GNU_GETTEXT([external])
AM_GNU_GETTEXT_VERSION([0.18.1])

AC_OUTPUT([
Makefile
src/Makefile
po/Makefile.in
data/biorhythm.desktop.in
data/Makefile
])
