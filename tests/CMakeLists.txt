add_executable(easy-gl-smoke-tests
        smoke/SmokeInitTests.cpp)

target_link_libraries(easy-gl-smoke-tests PRIVATE easy-gl::easy-gl)

add_test(NAME easy-gl-smoke-tests COMMAND easy-gl-smoke-tests)