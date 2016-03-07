#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.59])
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
PKG_PROG_PKG_CONFIG

PKG_PROG_PKG_CONFIG(0.16)

CPPFLAGS="$CPPFLAGS -std=c++0x"

# Checks for libraries.
CPPFLAGS="$CPPFLAGS -I/usr/local/include"
LIBS="$LIBS -L/usr/local/lib"
AC_CHECK_LIB([jpeg], jpeg_start_decompress)

# Checks for header files.

# Checks for typedefs, structures, and compiler characteristics.
AC_CHECK_HEADER_STDBOOL

# Checks for library functions.
PKG_CHECK_MODULES(libxml2, libxml-2.0 >= 2.8)
LIBS="$LIBS $libxml2_LIBS"
CPPFLAGS="$CPPFLAGS $libxml2_CFLAGS"

PKG_CHECK_MODULES(libpng, libpng >= 1.2)
LIBS="$LIBS $libpng_LIBS"
CPPFLAGS="$CPPFLAGS $libpng_CFLAGS"

######################
# Optional Libraries #
######################

have_sdl=no
PKG_CHECK_MODULES(SDL, sdl2 >= 2.0,
  [
    have_sdl=yes
    CPPFLAGS="$CPPFLAGS $SDL_CFLAGS"
    LIBS="$LIBS $SDL_LIBS"
  ], [
    have_sdl=no
  ])
AM_CONDITIONAL(HAVE_SDL2, test "x$have_sdl" = xyes)

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
    tests/gfx/Makefile
    tests/gtk/Makefile
    include/Makefile
    include/geek/Makefile
    libgeek.pc
])

AC_OUTPUT
