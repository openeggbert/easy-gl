if(CMAKE_SYSTEM_NAME STREQUAL "Emscripten")
    set(EMSCRIPTEN TRUE)
endif()

if(EMSCRIPTEN)
    # easy-gl (unlike meta-gl, which uses a std::terminate()-based Invalid
    # Input Contract) reports unsupported/missing GL features via C++
    # exceptions (see Exception.hpp, UnsupportedFeatureException). Emscripten
    # requires this to be explicitly opted into. Standalone easy-gl uses native
    # Wasm EH by default; an embedding application that needs Asyncify may select
    # the JS-lowered ABI. Applied before add_subdirectory(meta-gl) so every object
    # in the final link agrees on the model.
    set(EASYGL_EMSCRIPTEN_EXCEPTION_MODEL "WASM" CACHE STRING
        "Emscripten exception ABI: WASM or JS")
    set_property(CACHE EASYGL_EMSCRIPTEN_EXCEPTION_MODEL PROPERTY STRINGS WASM JS)
    if(EASYGL_EMSCRIPTEN_EXCEPTION_MODEL STREQUAL "WASM")
        add_compile_options(-fwasm-exceptions)
        add_link_options(-fwasm-exceptions)
    elseif(EASYGL_EMSCRIPTEN_EXCEPTION_MODEL STREQUAL "JS")
        add_compile_options(-fexceptions)
        add_link_options(-fexceptions -sDISABLE_EXCEPTION_CATCHING=0)
    else()
        message(FATAL_ERROR
            "EASYGL_EMSCRIPTEN_EXCEPTION_MODEL must be WASM or JS, got "
            "'${EASYGL_EMSCRIPTEN_EXCEPTION_MODEL}'")
    endif()
endif()

message(STATUS "easy-gl platform summary:")
message(STATUS "  System name      : ${CMAKE_SYSTEM_NAME}")
message(STATUS "  C++ compiler     : ${CMAKE_CXX_COMPILER_ID}")
message(STATUS "  Build shared     : ${BUILD_SHARED_LIBS}")
message(STATUS "  Build tests      : ${EASYGL_BUILD_TESTS}")
message(STATUS "  Build examples   : ${EASYGL_BUILD_EXAMPLES}")
