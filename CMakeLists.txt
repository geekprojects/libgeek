
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

include(GNUInstallDirs)

find_package(PkgConfig REQUIRED)

check_symbol_exists(memset_pattern4 "string.h" HAVE_MEMSET_PATTERN4)

pkg_check_modules(sigcpp REQUIRED sigc++-2.0)
pkg_check_modules(libxml2 REQUIRED libxml-2.0)
pkg_check_modules(freetype2 REQUIRED freetype2)
pkg_check_modules(libpng REQUIRED libpng)
pkg_check_modules(libjpeg REQUIRED libjpeg)

find_library(SQLITE3_LIBRARY NAMES sqlite3 )
find_library(z_LIBRARY NAMES z )

include_directories(
    ${CMAKE_SOURCE_DIR}/include
)

add_subdirectory(src/core)
add_subdirectory(src/fonts)
add_subdirectory(src/gfx)
add_subdirectory(include/geek)

set(PKGCONFIG_CFLAGS_LIST -I${CMAKE_INSTALL_PREFIX}/include ${sigcpp_CFLAGS} ${libxml2_CFLAGS} ${freetype2_CFLAGS} ${libjpeg_CFLAGS})
string(REPLACE ";" " " PKGCONFIG_CFLAGS "${PKGCONFIG_CFLAGS_LIST}")
set(PKGCONFIG_LDFLAGS_LIST -L${CMAKE_INSTALL_PREFIX}/lib -lgeek-core -lgeek-gfx -lgeek-fonts ${sigcpp_LDFLAGS} ${libxml2_LDFLAGS} ${freetype2_LDFLAGS} ${libjpeg_LDFLAGS})
string(REPLACE ";" " " PKGCONFIG_LDFLAGS "${PKGCONFIG_LDFLAGS_LIST}")

configure_file(libgeek.pc.in libgeek.pc @ONLY)
install(FILES ${CMAKE_BINARY_DIR}/libgeek.pc DESTINATION "lib${LIB_SUFFIX}/pkgconfig")


#
# Doxygen
#
find_package(Doxygen COMPONENTS dot)
if (DOXYGEN_FOUND)
    set(DOXYGEN_GENERATE_HTML YES)
    set(DOXYGEN_GENERATE_MAN NO)
    set(DOXYGEN_EXTRACT_ALL YES)

    doxygen_add_docs(
        doxygen
        ${PROJECT_SOURCE_DIR}/include
        COMMENT "Generate man pages"
    )
endif()

#
# CPack
#
include(InstallRequiredSystemLibraries)
set(CPACK_PACKAGE_CONTACT "Ian Parker <ian@geekprojects.com>")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/COPYING")
set(CPACK_PACKAGE_VERSION "${PROJECT_VERSION}")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Useful code from GeekProjects")
set(CPACK_PACKAGE_DESCRIPTION "A small library of various useful code.")

set(CPACK_DEBIAN_FILE_NAME DEB-DEFAULT)
set(CPACK_DEBIAN_PACKAGE_VERSION "${PROJECT_VERSION}")
set(CPACK_DEBIAN_PACKAGE_DEPENDS "libxml2-dev (>= 2.9), libpng-dev (>= 1.6), libjpeg-dev (>= 8c), sqlite3 (>= 3), libsqlite0-dev (>= 2.8), libsigc++-2.0-dev (>= 2), libfreetype6-dev (>= 2.10)")
set(CPACK_DEBIAN_PACKAGE_SHLIBDEPS ON)
set(CPACK_DEBIAN_PACKAGE_GENERATE_SHLIBS ON)
set(CPACK_DEBIAN_PACKAGE_CONTROL_STRICT_PERMISSION YES)
if (CPACK_GENERATOR MATCHES "DEB")
  # lintian complains changelog-file-missing-in-native-package, this fixes it,
  execute_process(COMMAND cp ${CMAKE_CURRENT_SOURCE_DIR}/debian/changelog ${CMAKE_CURRENT_SOURCE_DIR}/debian/changelog.bak)
  execute_process(COMMAND gzip -9 -f -n ${CMAKE_CURRENT_SOURCE_DIR}/debian/changelog)
  execute_process(COMMAND mv ${CMAKE_CURRENT_SOURCE_DIR}/debian/changelog.bak ${CMAKE_CURRENT_SOURCE_DIR}/debian/changelog)
  install(FILES debian/changelog.gz DESTINATION ${CMAKE_INSTALL_DOCDIR}/libgeek)
  install(FILES debian/copyright DESTINATION ${CMAKE_INSTALL_DOCDIR}/libgeek)
endif (CPACK_GENERATOR MATCHES "DEB")

include(CPack)

