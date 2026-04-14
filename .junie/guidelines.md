# easy-gl guidelines

- easy-gl is a toolkit-independent C++ wrapper over OpenGL and OpenGLES.
- Never expose SDL, GLFW, Qt, glad, or GLEW types in public headers.
- The library must not create windows or GL contexts.
- The host application owns context creation and platform integration.
- Prefer minimal compilable stubs over speculative large implementations.
- Keep the public API under include/easygl.
- Use C++20.
- Keep code warning-friendly for clang.
- OpenGL-only features must be capability-gated and treated as unsupported on OpenGLES platforms.
- Avoid unnecessary abstractions.
- If runtime GL loading is not ready yet, leave clear TODOs and preserve build success.