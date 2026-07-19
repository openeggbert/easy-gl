if(CMAKE_SYSTEM_NAME STREQUAL "Emscripten")
    set(EMSCRIPTEN TRUE)
endif()

if(EMSCRIPTEN)
    # easy-gl (unlike meta-gl, which uses a std::terminate()-based Invalid
    # Input Contract) reports unsupported/missing GL features via C++
    # exceptions (see Exception.hpp, UnsupportedFeatureException). Emscripten
    # requires this to be explicitly opted into via the modern Wasm exception
    # handling proposal (natively supported by Node/browsers this project
    # targets); applied globally (before add_subdirectory(meta-gl)) so every
    # object file in the link, including meta-gl, agrees on the exception
    # model. See tests/smoke/WebGLTests.cpp.
    add_compile_options(-fwasm-exceptions)
    add_link_options(-fwasm-exceptions)
endif()

message(STATUS "easy-gl platform summary:")
message(STATUS "  System name      : ${CMAKE_SYSTEM_NAME}")
message(STATUS "  C++ compiler     : ${CMAKE_CXX_COMPILER_ID}")
message(STATUS "  Build shared     : ${BUILD_SHARED_LIBS}")
message(STATUS "  Build tests      : ${EASYGL_BUILD_TESTS}")
message(STATUS "  Build examples   : ${EASYGL_BUILD_EXAMPLES}")
