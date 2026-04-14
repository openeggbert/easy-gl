# Integration Placeholder

This directory shows how host applications can integrate with `easy-gl`.

## GL Function Loading

`easy-gl` is toolkit-independent. It does not know how to load OpenGL functions by itself.
Instead, the host application must provide a `GetProcAddress` callback during `Device::initialize()`.

### SDL Integration

```cpp
#include <SDL3/SDL.h>
#include <easygl/easygl.hpp>

// ... after creating SDL_Window and SDL_GLContext
easygl::Device device;
device.initialize([](const char* name) {
    return SDL_GL_GetProcAddress(name);
});
```

### GLFW Integration

```cpp
#include <GLFW/glfw3.h>
#include <easygl/easygl.hpp>

// ... after creating GLFWwindow and making context current
easygl::Device device;
device.initialize([](const char* name) {
    return (void*)glfwGetProcAddress(name);
});
```

Note: Some platforms/compilers might require a `reinterpret_cast` when passing function pointers from GLFW to `easy-gl` as `void*`.
