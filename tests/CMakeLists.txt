FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/609281088cfefc76f9d0ce82e1ff6c30cc3591e5.zip
)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

add_executable(
    core_test
    core/dynamicarray.cpp
    core/tasks.cpp
)
add_executable(
    gfx_test
    gfx/tga.cpp
)

add_definitions(${sigcpp_CFLAGS} ${libxml2_CFLAGS})

target_link_libraries(
    core_test
    gtest_main
    geek-core
    ${sigcpp_LDFLAGS} ${libxml2_LIBRARIES} ${SQLITE3_LIBRARY} ${z_LIBRARY}
)

target_link_libraries(
        gfx_test
        gtest_main
        geek-gfx
        ${sigcpp_LDFLAGS} ${libxml2_LIBRARIES} ${SQLITE3_LIBRARY} ${z_LIBRARY}
)

include(GoogleTest)
gtest_discover_tests(core_test)
gtest_discover_tests(gfx_test)
