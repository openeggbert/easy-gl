add_executable(easy-gl-smoke-tests
        smoke/SmokeInitTests.cpp)
target_link_libraries(easy-gl-smoke-tests PRIVATE easy-gl::easy-gl)
easygl_apply_warnings(easy-gl-smoke-tests)

add_executable(easy-gl-resource-smoke-tests
        smoke/SmokeResourceTests.cpp)
target_link_libraries(easy-gl-resource-smoke-tests PRIVATE easy-gl::easy-gl)
easygl_apply_warnings(easy-gl-resource-smoke-tests)

add_test(NAME easy-gl-smoke-tests COMMAND easy-gl-smoke-tests)
add_test(NAME easy-gl-resource-smoke-tests COMMAND easy-gl-resource-smoke-tests)