#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT(Biorhythmus, 0.13, gabisoft@freesurf.ch)
AM_INIT_AUTOMAKE
AC_CONFIG_HEADERS([config.h])

AC_PROG_INTLTOOL(, no-xml)      # require >= 0.31 for autoreconf
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

# Checks for header files.

# Checks for typedefs, structures, and compiler characteristics.

# Checks for library functions.
AC_CHECK_FUNCS([floor])

GETTEXT_PACKAGE="$PACKAGE"
AC_SUBST(GETTEXT_PACKAGE)
AM_GLIB_GNU_GETTEXT
AM_GLIB_DEFINE_LOCALEDIR(LOCALEDIR)

AC_OUTPUT([
Makefile
src/Makefile
po/Makefile.in
])
