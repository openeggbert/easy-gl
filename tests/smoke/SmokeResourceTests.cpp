#include <easygl/easygl.hpp>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <string>

namespace
{
    struct FakeGlState
    {
        unsigned int next_buffer = 1;
        unsigned int next_shader = 10;
        unsigned int next_program = 100;
        unsigned int next_vertex_array = 200;
        unsigned int next_texture = 300;

        int shader_compile_status = 1;
        int program_link_status = 1;

        unsigned int last_buffer_data_target = 0;
        std::size_t last_buffer_data_size = 0;

        unsigned int last_vertex_attrib_index = 0;
        int last_vertex_attrib_size = 0;
        unsigned int last_vertex_attrib_type = 0;
        bool last_vertex_attrib_normalized = false;
        int last_vertex_attrib_stride = 0;
        std::uintptr_t last_vertex_attrib_offset = 0;
        unsigned int last_enabled_attrib_index = 0;

        int detach_shader_calls = 0;
        int delete_shader_calls = 0;

        int uniform4_calls = 0;
        int last_uniform_location = -1;
        float last_uniform4_values[4] = {0.0f, 0.0f, 0.0f, 0.0f};

        int pixel_store_calls = 0;
        int pixel_store_alignment = -1;
        int tex_wrap_s = -1;
        int tex_wrap_t = -1;
        int tex_min_filter = -1;
        int tex_mag_filter = -1;
        unsigned int last_active_texture = 0;
    };

    FakeGlState g_state;

    void reset_state()
    {
        g_state = FakeGlState{};
    }

    static void* fake_loader(const char* name)
    {
        const std::string n(name);

        if (n == "glGetString") {
            return (void*)+[](unsigned int value) -> const unsigned char* {
                if (value == 0x1F02) return (const unsigned char*)"3.3.0 fake";
                if (value == 0x1F00) return (const unsigned char*)"easy-gl";
                if (value == 0x1F01) return (const unsigned char*)"fake";
                if (value == 0x8B8C) return (const unsigned char*)"3.30";
                return (const unsigned char*)"";
            };
        }
        if (n == "glGetStringi") {
            return (void*)+[](unsigned int, unsigned int) -> const unsigned char* {
                return (const unsigned char*)"";
            };
        }
        if (n == "glGetIntegerv") {
            return (void*)+[](unsigned int pname, int* data) {
                if (pname == 0x821B) *data = 3;
                if (pname == 0x821C) *data = 3;
                if (pname == 0x821D) *data = 0;
                if (pname == 0x0D33) *data = 4096;
            };
        }

        if (n == "glGenBuffers") {
            return (void*)+[](int count, unsigned int* buffers) {
                for (int i = 0; i < count; ++i) {
                    buffers[i] = g_state.next_buffer++;
                }
            };
        }
        if (n == "glDeleteBuffers") {
            return (void*)+[](int, const unsigned int*) {};
        }
        if (n == "glBindBuffer") {
            return (void*)+[](unsigned int, unsigned int) {};
        }
        if (n == "glBufferData") {
            return (void*)+[](unsigned int target, long size, const void*, unsigned int) {
                g_state.last_buffer_data_target = target;
                g_state.last_buffer_data_size = static_cast<std::size_t>(size);
            };
        }
        if (n == "glBufferSubData") {
            return (void*)+[](unsigned int, long, long, const void*) {};
        }
        if (n == "glBindBufferBase") {
            return (void*)+[](unsigned int, unsigned int, unsigned int) {};
        }

        if (n == "glCreateShader") {
            return (void*)+[](unsigned int) -> unsigned int {
                return g_state.next_shader++;
            };
        }
        if (n == "glShaderSource") {
            return (void*)+[](unsigned int, int, const char* const*, const int*) {};
        }
        if (n == "glCompileShader") {
            return (void*)+[](unsigned int) {};
        }
        if (n == "glGetShaderiv") {
            return (void*)+[](unsigned int, unsigned int pname, int* params) {
                if (pname == 0x8B81) *params = g_state.shader_compile_status;
                if (pname == 0x8B84) *params = 0;
            };
        }
        if (n == "glGetShaderInfoLog") {
            return (void*)+[](unsigned int, int, int*, char*) {};
        }
        if (n == "glDeleteShader") {
            return (void*)+[](unsigned int) {
                ++g_state.delete_shader_calls;
            };
        }

        if (n == "glCreateProgram") {
            return (void*)+[]() -> unsigned int {
                return g_state.next_program++;
            };
        }
        if (n == "glAttachShader") {
            return (void*)+[](unsigned int, unsigned int) {};
        }
        if (n == "glDetachShader") {
            return (void*)+[](unsigned int, unsigned int) {
                ++g_state.detach_shader_calls;
            };
        }
        if (n == "glLinkProgram") {
            return (void*)+[](unsigned int) {};
        }
        if (n == "glGetProgramiv") {
            return (void*)+[](unsigned int, unsigned int pname, int* params) {
                if (pname == 0x8B82) *params = g_state.program_link_status;
                if (pname == 0x8B84) *params = 0;
            };
        }
        if (n == "glGetProgramInfoLog") {
            return (void*)+[](unsigned int, int, int*, char*) {};
        }
        if (n == "glUseProgram") {
            return (void*)+[](unsigned int) {};
        }
        if (n == "glDeleteProgram") {
            return (void*)+[](unsigned int) {};
        }

        if (n == "glGetUniformLocation") {
            return (void*)+[](unsigned int, const char*) -> int { return 0; };
        }
        if (n == "glUniform1i") {
            return (void*)+[](int, int) {};
        }
        if (n == "glUniform1f") {
            return (void*)+[](int, float) {};
        }
        if (n == "glUniform3f") {
            return (void*)+[](int, float, float, float) {};
        }
        if (n == "glUniform4f") {
            return (void*)+[](int location, float x, float y, float z, float w) {
                ++g_state.uniform4_calls;
                g_state.last_uniform_location = location;
                g_state.last_uniform4_values[0] = x;
                g_state.last_uniform4_values[1] = y;
                g_state.last_uniform4_values[2] = z;
                g_state.last_uniform4_values[3] = w;
            };
        }
        if (n == "glUniformMatrix4fv") {
            return (void*)+[](int, int, unsigned char, const float*) {};
        }

        if (n == "glGenVertexArrays") {
            return (void*)+[](int count, unsigned int* arrays) {
                for (int i = 0; i < count; ++i) {
                    arrays[i] = g_state.next_vertex_array++;
                }
            };
        }
        if (n == "glDeleteVertexArrays") {
            return (void*)+[](int, const unsigned int*) {};
        }
        if (n == "glBindVertexArray") {
            return (void*)+[](unsigned int) {};
        }
        if (n == "glEnableVertexAttribArray") {
            return (void*)+[](unsigned int index) {
                g_state.last_enabled_attrib_index = index;
            };
        }
        if (n == "glVertexAttribPointer") {
            return (void*)+[](unsigned int index, int size, unsigned int type, unsigned char normalized, int stride, const void* pointer) {
                g_state.last_vertex_attrib_index = index;
                g_state.last_vertex_attrib_size = size;
                g_state.last_vertex_attrib_type = type;
                g_state.last_vertex_attrib_normalized = normalized != 0;
                g_state.last_vertex_attrib_stride = stride;
                g_state.last_vertex_attrib_offset = reinterpret_cast<std::uintptr_t>(pointer);
            };
        }

        if (n == "glClear") {
            return (void*)+[](unsigned int) {};
        }
        if (n == "glClearColor") {
            return (void*)+[](float, float, float, float) {};
        }
        if (n == "glViewport") {
            return (void*)+[](int, int, int, int) {};
        }
        if (n == "glDrawArrays") {
            return (void*)+[](unsigned int, int, int) {};
        }
        if (n == "glDrawElements") {
            return (void*)+[](unsigned int, int, unsigned int, const void*) {};
        }

        if (n == "glGenTextures") {
            return (void*)+[](int count, unsigned int* textures) {
                for (int i = 0; i < count; ++i) {
                    textures[i] = g_state.next_texture++;
                }
            };
        }
        if (n == "glDeleteTextures") {
            return (void*)+[](int, const unsigned int*) {};
        }
        if (n == "glActiveTexture") {
            return (void*)+[](unsigned int texture) {
                g_state.last_active_texture = texture;
            };
        }
        if (n == "glPixelStorei") {
            return (void*)+[](unsigned int pname, int param) {
                if (pname == 0x0CF5) {
                    ++g_state.pixel_store_calls;
                    g_state.pixel_store_alignment = param;
                }
            };
        }
        if (n == "glBindTexture") {
            return (void*)+[](unsigned int, unsigned int) {};
        }
        if (n == "glTexImage2D") {
            return (void*)+[](unsigned int, int, int, int, int, int, unsigned int, unsigned int, const void*) {};
        }
        if (n == "glTexParameteri") {
            return (void*)+[](unsigned int, unsigned int pname, int param) {
                if (pname == 0x2800) g_state.tex_mag_filter = param;
                if (pname == 0x2801) g_state.tex_min_filter = param;
                if (pname == 0x2802) g_state.tex_wrap_s = param;
                if (pname == 0x2803) g_state.tex_wrap_t = param;
            };
        }

        if (n == "glEnable") {
            return (void*)+[](unsigned int) {};
        }
        if (n == "glDisable") {
            return (void*)+[](unsigned int) {};
        }
        if (n == "glBlendFunc") {
            return (void*)+[](unsigned int, unsigned int) {};
        }

        return nullptr;
    }

    void test_buffer_upload_target()
    {
        reset_state();
        easygl::Buffer buffer;
        buffer.create();

        constexpr float data[] = {0.0f, 1.0f, 2.0f};
        buffer.set_data(easygl::BufferTarget::ElementArray, data, sizeof(data));

        assert(g_state.last_buffer_data_target == 0x8893);
        assert(g_state.last_buffer_data_size == sizeof(data));
    }

    void test_vertex_attribute_layout()
    {
        reset_state();
        easygl::VertexArray vao;
        vao.create();
        vao.bind();

        vao.set_attribute(easygl::VertexAttribute{1, 3, easygl::DataType::Float, false, 6 * sizeof(float), 3 * sizeof(float), true});

        assert(g_state.last_vertex_attrib_index == 1);
        assert(g_state.last_vertex_attrib_size == 3);
        assert(g_state.last_vertex_attrib_type == 0x1406);
        assert(!g_state.last_vertex_attrib_normalized);
        assert(g_state.last_vertex_attrib_stride == static_cast<int>(6 * sizeof(float)));
        assert(g_state.last_vertex_attrib_offset == 3 * sizeof(float));
        assert(g_state.last_enabled_attrib_index == 1);
    }

    void test_texture_upload_sets_unpack_alignment_wrap_and_unit0_binding()
    {
        reset_state();

        easygl::Texture texture;
        texture.create();

        constexpr unsigned char pixel[4] = {255, 255, 255, 255};
        texture.set_image_2d(easygl::TextureTarget::Texture2D, 0, 1, 1, pixel);
        texture.bind(easygl::TextureTarget::Texture2D);

        assert(g_state.pixel_store_calls == 1);
        assert(g_state.pixel_store_alignment == 1);
        assert(g_state.tex_min_filter == 0x2601);
        assert(g_state.tex_mag_filter == 0x2601);
        assert(g_state.tex_wrap_s == 0x812F);
        assert(g_state.tex_wrap_t == 0x812F);
        assert(g_state.last_active_texture == 0x84C0);
    }

    void test_program_owned_shaders_cleanup_after_link()
    {
        reset_state();

        easygl::Shader vertex_shader(easygl::ShaderStage::Vertex);
        vertex_shader.create();
        easygl::Shader fragment_shader(easygl::ShaderStage::Fragment);
        fragment_shader.create();

        easygl::Program program;
        program.attach_owned(vertex_shader);
        program.attach_owned(fragment_shader);

        assert(!vertex_shader.is_created());
        assert(!fragment_shader.is_created());

        program.link();
        assert(program.is_linked());
        assert(g_state.detach_shader_calls == 2);
        assert(g_state.delete_shader_calls == 2);
    }

    void test_program_owned_shaders_cleanup_on_destroy_when_link_fails()
    {
        reset_state();
        g_state.program_link_status = 0;

        easygl::Shader vertex_shader(easygl::ShaderStage::Vertex);
        vertex_shader.create();

        easygl::Program program;
        program.attach_owned(vertex_shader);
        program.link();

        assert(!program.is_linked());
        assert(g_state.delete_shader_calls == 0);

        program.destroy();
        assert(g_state.detach_shader_calls == 1);
        assert(g_state.delete_shader_calls == 1);
    }

    void test_program_compile_from_sources_and_uniform4()
    {
        reset_state();

        const std::string vertex_source = R"(
            #version 330 core
            layout(location = 0) in vec3 aPos;
            void main() { gl_Position = vec4(aPos, 1.0); }
        )";
        const std::string fragment_source = R"(
            #version 330 core
            uniform vec4 uColor;
            out vec4 FragColor;
            void main() { FragColor = uColor; }
        )";

        easygl::Program shader(vertex_source, fragment_source);
        assert(shader.is_linked());

        const int color_location = shader.uniform_location("uColor");
        shader.set_uniform(color_location, 0.1f, 0.2f, 0.3f, 0.4f);

        assert(g_state.uniform4_calls == 1);
        assert(g_state.last_uniform_location == color_location);
        assert(g_state.last_uniform4_values[0] == 0.1f);
        assert(g_state.last_uniform4_values[1] == 0.2f);
        assert(g_state.last_uniform4_values[2] == 0.3f);
        assert(g_state.last_uniform4_values[3] == 0.4f);
    }

    void test_program_compile_from_sources_reports_errors()
    {
        reset_state();
        g_state.shader_compile_status = 0;

        bool compile_exception_thrown = false;
        try {
            easygl::Program shader("void main(){}", "void main(){}");
            static_cast<void>(shader);
        } catch (const easygl::Exception& e) {
            compile_exception_thrown = std::string(e.what()).find("Vertex shader compilation failed") != std::string::npos;
        }
        assert(compile_exception_thrown);

        reset_state();
        g_state.program_link_status = 0;

        bool link_exception_thrown = false;
        try {
            easygl::Program shader("void main(){}", "void main(){}");
            static_cast<void>(shader);
        } catch (const easygl::Exception& e) {
            link_exception_thrown = std::string(e.what()).find("Program linking failed") != std::string::npos;
        }
        assert(link_exception_thrown);
    }
}

int main()
{
    try {
        easygl::Device device;
        device.initialize(fake_loader);

        test_buffer_upload_target();
        test_vertex_attribute_layout();
        test_texture_upload_sets_unpack_alignment_wrap_and_unit0_binding();
        test_program_owned_shaders_cleanup_after_link();
        test_program_owned_shaders_cleanup_on_destroy_when_link_fails();
        test_program_compile_from_sources_and_uniform4();
        test_program_compile_from_sources_reports_errors();
        std::cout << "easy-gl resource smoke tests passed\n";
    } catch (const std::exception& e) {
        std::cerr << "Test failed: " << e.what() << "\n";
        return 1;
    }
    return 0;
}
