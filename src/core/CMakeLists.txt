
add_subdirectory(sha)

ADD_LIBRARY(geek-core SHARED
data.cpp           logger.cpp         sha.cpp            thread-pthread.cpp timers.cpp
database.cpp       matrix.cpp         string.cpp         thread-pthread.h   utf8.h
file.cpp           random.cpp         tasks.cpp          thread.cpp         xml.cpp
)

add_definitions(${sigcpp_CFLAGS} ${libxml2_CFLAGS})

target_link_libraries(geek-core ${sigcpp_LDFLAGS} ${libxml2_LIBRARIES} ${SQLITE3_LIBRARY} ${z_LIBRARY} sha)

install(TARGETS geek-core DESTINATION lib)

