#include "easygl/Device.hpp"
#include "platform/GlFunctions.hpp"
#include <utility>
#include <stdexcept>

namespace easygl
{
    Device::Device() : Device(Config{})
    {
    }

    Device::Device(Config config)
        : config_(config)
        , initialized_(false)
    {
    }

    void Device::initialize(GLGetProcAddressFn loader)
    {
        if (initialized_)
        {
            return;
        }

        if (loader == nullptr)
        {
            throw Exception("GL loader callback is null.");
        }

        loader_ = loader;

        // 1. Initialize internal GL function pointers (Bootstrap)
        // We need at least glGetString to identify the context.
        if (!platform::initialize_gl_functions(platform::g_gl, loader_))
        {
            throw Exception("Failed to bootstrap GL functions.");
        }

        // 2. Query Context Information
        const char* vendor = reinterpret_cast<const char*>(platform::g_gl.GetString(platform::GL_VENDOR));
        const char* renderer = reinterpret_cast<const char*>(platform::g_gl.GetString(platform::GL_RENDERER));
        const char* version = reinterpret_cast<const char*>(platform::g_gl.GetString(platform::GL_VERSION));
        const char* sl_version = reinterpret_cast<const char*>(platform::g_gl.GetString(platform::GL_SHADING_LANGUAGE_VERSION));

        platform::GLint major = 0;
        platform::GLint minor = 0;
        platform::g_gl.GetIntegerv(platform::GL_MAJOR_VERSION, &major);
        platform::g_gl.GetIntegerv(platform::GL_MINOR_VERSION, &minor);

        ContextInfo info;
        info.api = ApiKind::OpenGL; // TODO: Detect OpenGLES
        info.major = major;
        info.minor = minor;
        info.vendor = vendor ? vendor : "";
        info.renderer = renderer ? renderer : "";
        info.version_string = version ? version : "";
        info.shading_language_version = sl_version ? sl_version : "";

        capabilities_.set_context_info(std::move(info));
        
        // 3. Detect features based on version and extensions
        capabilities_.detect_common_features();

        // 4. Query Hardware Limits
        // TODO: Query GL limits (e.g., GL_MAX_TEXTURE_SIZE) using glGetIntegerv

        initialized_ = true;
    }

    bool Device::is_initialized() const noexcept
    {
        return initialized_;
    }

    const Config& Device::config() const noexcept
    {
        return config_;
    }

    const Capabilities& Device::capabilities() const noexcept
    {
        return capabilities_;
    }

    bool Device::supports(Feature feature) const
    {
        return capabilities_.supports(feature);
    }

    void Device::require(Feature feature) const
    {
        if (!supports(feature))
        {
            if (config_.throw_on_missing_feature)
            {
                throw UnsupportedFeatureException("Requested feature is not supported by the current device.");
            }
        }
    }

    void Device::clear(ClearFlags flags)
    {
        platform::GLbitfield mask = 0;
        if ((static_cast<u32>(flags) & static_cast<u32>(ClearFlags::Color)) != 0) mask |= platform::GL_COLOR_BUFFER_BIT;
        if ((static_cast<u32>(flags) & static_cast<u32>(ClearFlags::Depth)) != 0) mask |= platform::GL_DEPTH_BUFFER_BIT;
        if ((static_cast<u32>(flags) & static_cast<u32>(ClearFlags::Stencil)) != 0) mask |= platform::GL_STENCIL_BUFFER_BIT;
        
        platform::g_gl.Clear(mask);
    }

    void Device::set_clear_color(float r, float g, float b, float a)
    {
        platform::g_gl.ClearColor(r, g, b, a);
    }

    void Device::set_viewport(int x, int y, int width, int height)
    {
        platform::g_gl.Viewport(x, y, width, height);
    }

    void Device::draw_arrays(PrimitiveType primitive, int first, int count)
    {
        platform::GLenum mode = 0;
        switch (primitive)
        {
            case PrimitiveType::Triangles:      mode = platform::GL_TRIANGLES; break;
            case PrimitiveType::TriangleStrip: mode = platform::GL_TRIANGLE_STRIP; break;
            case PrimitiveType::TriangleFan:   mode = platform::GL_TRIANGLE_FAN; break;
            case PrimitiveType::Lines:          mode = platform::GL_LINES; break;
            case PrimitiveType::LineStrip:      mode = platform::GL_LINE_STRIP; break;
            case PrimitiveType::LineLoop:       mode = platform::GL_LINE_LOOP; break;
            case PrimitiveType::Points:         mode = platform::GL_POINTS; break;
        }
        platform::g_gl.DrawArrays(mode, first, count);
    }
}
