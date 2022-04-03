
ADD_LIBRARY(geek-gfx SHARED
    drawable.cpp
    highdpi.cpp
    surface.cpp
    colour.cpp
)

if (HAVE_MEMSET_PATTERN4)
    target_compile_definitions(geek-gfx PRIVATE "HAVE_MEMSET_PATTERN4")
endif()

add_definitions(${libpng_CFLAGS} ${libjpeg_CFLAGS})

target_link_libraries(geek-gfx ${libpng_LDFLAGS} ${libjpeg_LDFLAGS} geek-core)
set_target_properties(geek-gfx PROPERTIES VERSION ${PROJECT_VERSION})

install(TARGETS geek-gfx DESTINATION lib)

