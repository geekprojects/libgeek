
ADD_LIBRARY(geek-gfx SHARED
    drawable.cpp
    highdpi.cpp
    surface.cpp
)

add_definitions(${libpng_CFLAGS})

target_link_libraries(geek-gfx ${libpng_LDFLAGS} ${jpeg_LIBRARY} geek-core)

install(TARGETS geek-gfx DESTINATION lib)

