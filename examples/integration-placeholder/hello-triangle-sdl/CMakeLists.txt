find_package(SDL3 REQUIRED)

add_executable(hello-triangle-sdl
        HelloTriangle.cpp
)

target_link_libraries(hello-triangle-sdl
        PRIVATE
        easy-gl::easy-gl
        SDL3::SDL3
)

target_compile_features(hello-triangle-sdl PRIVATE cxx_std_20)

if(MSVC)
    target_compile_options(hello-triangle-sdl PRIVATE /W4)
else()
    target_compile_options(hello-triangle-sdl PRIVATE
            -Wall
            -Wextra
            -Wpedantic
    )
endif()