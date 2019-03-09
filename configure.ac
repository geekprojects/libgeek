#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.59])
AC_INIT([libgeek], [0.0.1], [libgeek@geekprojects.com])
AC_CONFIG_MACRO_DIR([.])
AM_INIT_AUTOMAKE([foreign -Wall -Werror])
AC_CONFIG_SRCDIR([include/geek/core-compiler.h])
AC_CONFIG_HEADERS([include/config.h])

# Checks for programs.
AM_PROG_AR
AC_PROG_CXX
AC_PROG_CC
AC_PROG_LIBTOOL
AC_PROG_RANLIB
AX_PTHREAD
AX_LIB_SQLITE3([3.0.0])
PKG_PROG_PKG_CONFIG(0.16)

AC_CHECK_FUNCS([memset_pattern4])

CPPFLAGS="$CPPFLAGS -std=c++0x"

# Checks for libraries.
CPPFLAGS="$CPPFLAGS -I/usr/local/include $SQLITE3_CFLAGS"
LIBS="$LIBS -L/usr/local/lib -ljpeg $SQLITE3_LDFLAGS"

# Checks for header files.

# Checks for typedefs, structures, and compiler characteristics.
AC_CHECK_HEADER_STDBOOL

# Checks for library functions.
AX_PTHREAD(, [AC_MSG_ERROR([libpthread is required])])
LIBS="$LIBS $PTHREAD_LIBS -lpthread"
CPPFLAGS="$CPPFLAGS $PTHREAD_CFLAGS"

PKG_CHECK_MODULES(libxml2, libxml-2.0 >= 2.8)
LIBS="$LIBS $libxml2_LIBS"
CPPFLAGS="$CPPFLAGS $libxml2_CFLAGS"

PKG_CHECK_MODULES(libpng, libpng >= 1.2)
LIBS="$LIBS $libpng_LIBS"
CPPFLAGS="$CPPFLAGS $libpng_CFLAGS"

CPPFLAGS="$CPPFLAGS -Wall -Werror -Werror=sign-compare"

AC_CONFIG_FILES([
    Makefile
    src/Makefile
    src/core/Makefile
    src/core/sha/Makefile
    src/gfx/Makefile
    tests/Makefile
    tests/core/Makefile
    tests/gfx/Makefile
    include/Makefile
    include/geek/Makefile
    libgeek.pc
])

AC_OUTPUT
