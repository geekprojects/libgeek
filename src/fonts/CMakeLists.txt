
ADD_LIBRARY(geek-fonts SHARED
    fontface.cpp
    fontfamily.cpp
    fonthandle.cpp
    fontmanager.cpp
)

add_definitions(${freetype2_CFLAGS})

target_link_libraries(geek-fonts ${freetype2_LDFLAGS} geek-core)
set_target_properties(geek-fonts PROPERTIES VERSION ${PROJECT_VERSION})

install(TARGETS geek-fonts DESTINATION lib)

