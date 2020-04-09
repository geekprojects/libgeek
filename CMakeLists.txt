
cmake_minimum_required(VERSION 3.10)

include(CheckSymbolExists)

# set the project name
project(libgeek VERSION 1.0)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

SET(FLAGS_COMMON "-Wall -Werror -Wno-nullability-completeness")
SET(CMAKE_C_FLAGS "${FLAGS_COMMON}")
SET(CMAKE_CXX_FLAGS "${FLAGS_COMMON}")
SET(CMAKE_C_FLAGS_DEBUG "${FLAGS_COMMON} -O0 -g  -fsanitize=address -fsanitize-address-use-after-scope -fno-omit-frame-pointer")
SET(CMAKE_CXX_FLAGS_DEBUG "${FLAGS_COMMON} -O0 -g -fsanitize=address -fsanitize-address-use-after-scope -fno-omit-frame-pointer")
SET(CMAKE_C_FLAGS_RELEASE "${FLAGS_COMMON} -O3")
SET(CMAKE_CXX_FLAGS_RELEASE "${FLAGS_COMMON} -O3")

find_package(PkgConfig REQUIRED)

check_symbol_exists(memset_pattern4 "string.h" HAVE_MEMSET_PATTERN4)

pkg_check_modules(sigcpp REQUIRED sigc++-2.0)
pkg_check_modules(libxml2 REQUIRED libxml-2.0)
pkg_check_modules(freetype2 REQUIRED freetype2)
pkg_check_modules(libpng REQUIRED libpng)

find_library(SQLITE3_LIBRARY NAMES sqlite3 )
find_library(z_LIBRARY NAMES z )
find_library(jpeg_LIBRARY NAMES jpeg )

include_directories(
    ${CMAKE_SOURCE_DIR}/include
    /usr/local/include
)

add_subdirectory(src/core)
add_subdirectory(src/fonts)
add_subdirectory(src/gfx)
add_subdirectory(include/geek)

set(PKGCONFIG_CFLAGS_LIST -I${CMAKE_INSTALL_PREFIX}/include ${sigcpp_CFLAGS} ${libxml2_CFLAGS} ${freetype2_CFLAGS})
string(REPLACE ";" " " PKGCONFIG_CFLAGS "${PKGCONFIG_CFLAGS_LIST}")
set(PKGCONFIG_LDFLAGS_LIST -L${CMAKE_INSTALL_PREFIX}/lib -lgeek-core -lgeek-gfx -lgeek-fonts ${sigcpp_LDFLAGS} ${libxml2_LDFLAGS} ${freetype2_LDFLAGS})
string(REPLACE ";" " " PKGCONFIG_LDFLAGS "${PKGCONFIG_LDFLAGS_LIST}")

configure_file(libgeek.pc.in libgeek.pc @ONLY)
install(FILES ${CMAKE_BINARY_DIR}/libgeek.pc DESTINATION "lib${LIB_SUFFIX}/pkgconfig")

