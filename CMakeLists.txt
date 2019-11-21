
cmake_minimum_required(VERSION 3.10)

# set the project name
project(libgeek VERSION 1.0)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

find_package(PkgConfig REQUIRED)

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

configure_file(libgeek.pc.in libgeek.pc @ONLY)
install(FILES libgeek.pc DESTINATION "lib${LIB_SUFFIX}/pkgconfig")

