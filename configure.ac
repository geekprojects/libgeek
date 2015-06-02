#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([libgeek], [0.0.1], [libgeek@geekprojects.com])
AM_INIT_AUTOMAKE([-Wall -Werror])
AC_CONFIG_SRCDIR([include/geek/gtk-chart.h])
AC_CONFIG_HEADERS([include/config.h])

# Checks for programs.
AM_PROG_AR
AC_PROG_CXX
AC_PROG_CC
AC_PROG_LIBTOOL
AC_PROG_RANLIB

PKG_PROG_PKG_CONFIG(0.16)

# Checks for libraries.
AC_CHECK_LIB([png], png_read_png)
AC_CHECK_LIB([jpeg], jpeg_start_decompress)

# Checks for header files.

# Checks for typedefs, structures, and compiler characteristics.
AC_CHECK_HEADER_STDBOOL

# Checks for library functions.

LIBS="`xml2-config --libs` $LIBS"
CPPFLAGS="`xml2-config --cflags` $CPPFLAGS"

######################
# Optional Libraries #
######################

# Check for gtkmm
AC_MSG_CHECKING([for gtkmm 3.0])
have_gtkmm=no
PKG_CHECK_EXISTS(
    gtkmm-3.0,
    have_gtkmm=yes
)
AC_MSG_RESULT([$have_gtkmm])

AM_CONDITIONAL(HAVE_GTKMM, test "x$have_gtkmm" = xyes)


AC_CONFIG_FILES([
    Makefile
    src/Makefile
    src/core/Makefile
    src/gtk/Makefile
    src/gfx/Makefile
    tools/Makefile
    tools/noise/Makefile
    tests/Makefile
    tests/core/Makefile
    tests/gtk/Makefile
    include/Makefile
    include/geek/Makefile
])

AC_OUTPUT
