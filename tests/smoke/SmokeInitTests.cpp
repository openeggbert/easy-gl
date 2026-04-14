#include <easygl/easygl.hpp>
#include <iostream>
#include <cassert>

static void* dummy_loader(const char*)
{
    return nullptr;
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

        std::cout << "easy-gl smoke test passed\n";
    }
    catch (const std::exception& ex)
    {
        std::cerr << "easy-gl smoke test failed: " << ex.what() << '\n';
        return 1;
    }

    return 0;
}