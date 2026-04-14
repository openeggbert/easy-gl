#include <easygl/easygl.hpp>
#include <iostream>
#include <cassert>

static void* dummy_loader(const char* name)
{
    std::string n(name);
    if (n == "glGetString") {
        return (void*)+[](unsigned int name) -> const unsigned char* {
            if (name == 0x1F02) return (const unsigned char*)"3.3.0 easy-gl stub";
            if (name == 0x1F00) return (const unsigned char*)"easy-gl";
            if (name == 0x1F01) return (const unsigned char*)"stub";
            if (name == 0x8B8C) return (const unsigned char*)"3.30";
            return (const unsigned char*)"";
        };
    }
    if (n == "glGetStringi") {
        return (void*)+[](unsigned int, unsigned int) -> const unsigned char* {
            return (const unsigned char*)"";
        };
    }
    if (n == "glGetIntegerv") {
        return (void*)+[](unsigned int name, int* data) {
            if (name == 0x821B) *data = 3; // MAJOR
            if (name == 0x821C) *data = 3; // MINOR
            if (name == 0x821D) *data = 0; // NUM_EXTENSIONS
            if (name == 0x0D33) *data = 1024; // MAX_TEXTURE_SIZE
        };
    }
    // Return a dummy non-null pointer for other required functions
    return (void*)0xDEADBEEF;
}

int main()
{
    easygl::Device device;

    try
    {
        device.initialize(dummy_loader);
        
        // Verify basic capability logic (based on the stubbed 3.3 OpenGL context)
        const auto& caps = device.capabilities();
        
        std::cout << "Context Info: " << caps.context_info().version_string << "\n";
        
        // OpenGL 3.3 should support VertexArrayObject
        assert(device.supports(easygl::Feature::VertexArrayObject));
        std::cout << "VAO supported: " << (device.supports(easygl::Feature::VertexArrayObject) ? "yes" : "no") << "\n";
        
        // OpenGL 3.3 should support FramebufferObject
        assert(device.supports(easygl::Feature::FramebufferObject));
        
        // OpenGL 3.3 should NOT support ComputeShader (introduced in 4.3)
        assert(!device.supports(easygl::Feature::ComputeShader));
        std::cout << "ComputeShader supported: " << (device.supports(easygl::Feature::ComputeShader) ? "yes" : "no") << "\n";

        // GLES Detection Test
        {
            easygl::Device glesDevice;
            glesDevice.initialize([](const char* name) -> void* {
                std::string n(name);
                if (n == "glGetString") {
                    return (void*)+[](unsigned int name) -> const unsigned char* {
                        if (name == 0x1F02) return (const unsigned char*)"OpenGL ES 3.1";
                        if (name == 0x1F00) return (const unsigned char*)"ARM";
                        if (name == 0x1F01) return (const unsigned char*)"Mali";
                        if (name == 0x8B8C) return (const unsigned char*)"310 es";
                        return (const unsigned char*)"";
                    };
                }
                if (n == "glGetIntegerv") {
                    return (void*)+[](unsigned int name, int* data) {
                        if (name == 0x821B) *data = 3; // MAJOR
                        if (name == 0x821C) *data = 1; // MINOR
                        if (name == 0x821D) *data = 0; // NUM_EXTENSIONS
                        if (name == 0x0D33) *data = 1024; // MAX_TEXTURE_SIZE
                    };
                }
                return (void*)0xDEADBEEF;
            });
            assert(glesDevice.capabilities().context_info().is_opengles());
            assert(glesDevice.capabilities().context_info().major == 3);
            assert(glesDevice.capabilities().context_info().minor == 1);
            assert(glesDevice.supports(easygl::Feature::ComputeShader)); // GLES 3.1 has ComputeShader
            std::cout << "GLES Detection worked: version 3.1, ComputeShader supported: yes\n";
        }

        // Extension Detection Test
        {
            easygl::Device extDevice;
            extDevice.initialize([](const char* name) -> void* {
                std::string n(name);
                if (n == "glGetString") {
                    return (void*)+[](unsigned int name) -> const unsigned char* {
                        if (name == 0x1F02) return (const unsigned char*)"3.3.0";
                        if (name == 0x1F00) return (const unsigned char*)"Intel";
                        if (name == 0x1F01) return (const unsigned char*)"HD 4000";
                        if (name == 0x8B8C) return (const unsigned char*)"3.30";
                        return (const unsigned char*)"";
                    };
                }
                if (n == "glGetStringi") {
                    return (void*)+[](unsigned int name, unsigned int index) -> const unsigned char* {
                        if (name == 0x1F03 && index == 0) return (const unsigned char*)"GL_ARB_compute_shader";
                        return (const unsigned char*)"";
                    };
                }
                if (n == "glGetIntegerv") {
                    return (void*)+[](unsigned int name, int* data) {
                        if (name == 0x821B) *data = 3; // MAJOR
                        if (name == 0x821C) *data = 3; // MINOR
                        if (name == 0x821D) *data = 1; // NUM_EXTENSIONS
                        if (name == 0x0D33) *data = 1024; // MAX_TEXTURE_SIZE
                    };
                }
                return (void*)0xDEADBEEF;
            });
            assert(extDevice.supports(easygl::Feature::ComputeShader));
            std::cout << "Extension Detection worked: ComputeShader supported: yes\n";
        }

        std::cout << "easy-gl smoke test passed\n";
    }
    catch (const std::exception& ex)
    {
        std::cerr << "easy-gl smoke test failed: " << ex.what() << '\n';
        return 1;
    }

    return 0;
}