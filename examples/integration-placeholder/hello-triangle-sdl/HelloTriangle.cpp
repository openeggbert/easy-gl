#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <easygl/easygl.hpp>
#include <iostream>
#include <vector>

namespace
{
    constexpr int SCR_WIDTH = 800;
    constexpr int SCR_HEIGHT = 600;

    const char* vertexShaderSource = R"(#version 330 core
layout (location = 0) in vec3 aPos;
void main()
{
    gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);
})";

    const char* fragmentShaderSource = R"(#version 330 core
out vec4 FragColor;
void main()
{
    FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);
})";
}

int main(int, char**)
{
    if (!SDL_Init(SDL_INIT_VIDEO))
    {
        std::cerr << "SDL_Init failed: " << SDL_GetError() << '\n';
        return 1;
    }

    // Request OpenGL 3.3 Core Profile.
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

#if defined(__APPLE__)
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG);
#endif

    SDL_Window* window = SDL_CreateWindow(
        "Hello Triangle - SDL3 + easy-gl",
        SCR_WIDTH,
        SCR_HEIGHT,
        SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);

    if (window == nullptr)
    {
        std::cerr << "SDL_CreateWindow failed: " << SDL_GetError() << '\n';
        SDL_Quit();
        return 1;
    }

    SDL_GLContext glContext = SDL_GL_CreateContext(window);
    if (glContext == nullptr)
    {
        std::cerr << "SDL_GL_CreateContext failed: " << SDL_GetError() << '\n';
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    if (!SDL_GL_MakeCurrent(window, glContext))
    {
        std::cerr << "SDL_GL_MakeCurrent failed: " << SDL_GetError() << '\n';
        SDL_GL_DestroyContext(glContext);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    // Optional vsync.
    if (!SDL_GL_SetSwapInterval(1)) {
        std::cerr << "Warning: SDL_GL_SetSwapInterval failed: " << SDL_GetError() << '\n';
    }

    // easy-gl initialization and main loop in a scope to ensure 
    // GL resources are destroyed before the context is destroyed.
    {
        easygl::Device device;
        try {
            device.initialize(reinterpret_cast<easygl::GLGetProcAddressFn>(SDL_GL_GetProcAddress));
        } catch (const easygl::Exception& e) {
            std::cerr << "easy-gl initialization failed: " << e.what() << '\n';
            SDL_GL_DestroyContext(glContext);
            SDL_DestroyWindow(window);
            SDL_Quit();
            return 1;
        }

        const auto& info = device.capabilities().context_info();
        std::cout << "GL_VENDOR   = " << info.vendor << '\n';
        std::cout << "GL_RENDERER = " << info.renderer << '\n';
        std::cout << "GL_VERSION  = " << info.version_string << '\n';

        constexpr float vertices[] = {
            -0.5f, -0.5f, 0.0f,
             0.5f, -0.5f, 0.0f,
             0.0f,  0.5f, 0.0f
        };

        easygl::Shader vertexShader(easygl::ShaderStage::Vertex);
        vertexShader.compile_from_source(vertexShaderSource);
        if (!vertexShader.is_compiled()) {
            std::cerr << "Vertex Shader compilation failed:\n" << vertexShader.info_log() << '\n';
            return 1;
        }

        easygl::Shader fragmentShader(easygl::ShaderStage::Fragment);
        fragmentShader.compile_from_source(fragmentShaderSource);
        if (!fragmentShader.is_compiled()) {
            std::cerr << "Fragment Shader compilation failed:\n" << fragmentShader.info_log() << '\n';
            return 1;
        }

        easygl::Program shaderProgram;
        shaderProgram.attach(vertexShader);
        shaderProgram.attach(fragmentShader);
        shaderProgram.link();
        if (!shaderProgram.is_linked()) {
            std::cerr << "Program linking failed:\n" << shaderProgram.info_log() << '\n';
            return 1;
        }

        easygl::VertexArray vao;
        vao.create();

        easygl::Buffer vbo;
        vbo.create();

        vao.bind();
        vbo.bind(easygl::BufferTarget::Array);
        vbo.set_data(vertices, sizeof(vertices));

        // Set up vertex attributes (layout)
        vao.set_attribute_pointer(0, 3, easygl::DataType::Float, false, 3 * sizeof(float), nullptr);
        vao.enable_attribute(0);

        int drawableWidth = 0;
        int drawableHeight = 0;
        SDL_GetWindowSizeInPixels(window, &drawableWidth, &drawableHeight);
        device.set_viewport(0, 0, drawableWidth, drawableHeight);

        bool running = true;
        while (running)
        {
            SDL_Event event;
            while (SDL_PollEvent(&event))
            {
                if (event.type == SDL_EVENT_QUIT)
                {
                    running = false;
                }
                else if (event.type == SDL_EVENT_KEY_DOWN)
                {
                    if (event.key.key == SDLK_ESCAPE)
                    {
                        running = false;
                    }
                }
                else if (event.type == SDL_EVENT_WINDOW_PIXEL_SIZE_CHANGED)
                {
                    device.set_viewport(0, 0, event.window.data1, event.window.data2);
                }
            }

            device.set_clear_color(0.2f, 0.3f, 0.3f, 1.0f);
            device.clear(easygl::ClearFlags::Color);

            shaderProgram.use();
            vao.bind();
            device.draw_arrays(easygl::PrimitiveType::Triangles, 0, 3);

            SDL_GL_SwapWindow(window);
        }
    }

    SDL_GL_DestroyContext(glContext);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}