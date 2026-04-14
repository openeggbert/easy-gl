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
        const char* vendor_ptr = reinterpret_cast<const char*>(platform::g_gl.GetString(platform::GL_VENDOR));
        const char* renderer_ptr = reinterpret_cast<const char*>(platform::g_gl.GetString(platform::GL_RENDERER));
        const char* version_ptr = reinterpret_cast<const char*>(platform::g_gl.GetString(platform::GL_VERSION));
        const char* sl_version_ptr = reinterpret_cast<const char*>(platform::g_gl.GetString(platform::GL_SHADING_LANGUAGE_VERSION));

        std::string vendor = vendor_ptr ? vendor_ptr : "";
        std::string renderer = renderer_ptr ? renderer_ptr : "";
        std::string version_string = version_ptr ? version_ptr : "";
        std::string shading_language_version = sl_version_ptr ? sl_version_ptr : "";

        ApiKind api = ApiKind::OpenGL;
        if (version_string.find("OpenGL ES") != std::string::npos)
        {
            api = ApiKind::OpenGLES;
        }

        platform::GLint major = 0;
        platform::GLint minor = 0;
        
        // glGetIntegerv(GL_MAJOR_VERSION) is only for GL 3.0+ / GLES 3.0+
        // For older versions we should ideally parse the version string.
        // But for a minimal implementation, let's try glGetIntegerv and if it's 0, try parsing.
        platform::g_gl.GetIntegerv(platform::GL_MAJOR_VERSION, &major);
        platform::g_gl.GetIntegerv(platform::GL_MINOR_VERSION, &minor);

        if (major == 0)
        {
            // Simple parsing of "Major.Minor"
            const char* v = version_string.c_str();
            if (api == ApiKind::OpenGLES)
            {
                // Version string format: "OpenGL ES <major>.<minor> <vendor-specific info>"
                if (version_string.length() > 10) v += 10;
            }
            if (v && *v >= '0' && *v <= '9')
            {
                major = *v - '0';
                if (*(v + 1) == '.' && *(v + 2) >= '0' && *(v + 2) <= '9')
                {
                    minor = *(v + 2) - '0';
                }
            }
        }

        std::vector<std::string> extensions;
        if (platform::g_gl.GetStringi && major >= 3)
        {
            platform::GLint num_extensions = 0;
            platform::g_gl.GetIntegerv(platform::GL_NUM_EXTENSIONS, &num_extensions);
            for (int i = 0; i < num_extensions; ++i)
            {
                const char* ext = reinterpret_cast<const char*>(platform::g_gl.GetStringi(platform::GL_EXTENSIONS, static_cast<platform::GLuint>(i)));
                if (ext) extensions.push_back(ext);
            }
        }
        else if (platform::g_gl.GetString)
        {
            const char* ext_ptr = reinterpret_cast<const char*>(platform::g_gl.GetString(platform::GL_EXTENSIONS));
            if (ext_ptr)
            {
                std::string s(ext_ptr);
                std::string delimiter = " ";
                size_t pos = 0;
                while ((pos = s.find(delimiter)) != std::string::npos) {
                    std::string token = s.substr(0, pos);
                    if (!token.empty()) extensions.push_back(token);
                    s.erase(0, pos + delimiter.length());
                }
                if (!s.empty()) extensions.push_back(s);
            }
        }

        ContextInfo info;
        info.api = api;
        info.major = major;
        info.minor = minor;
        info.vendor = vendor;
        info.renderer = renderer;
        info.version_string = version_string;
        info.shading_language_version = shading_language_version;
        info.extensions = std::move(extensions);

        capabilities_.set_context_info(std::move(info));
        
        // 3. Detect features based on version and extensions
        capabilities_.detect_common_features();

        // 4. Validate required features for HelloTriangle
        if (!capabilities_.supports(Feature::VertexArrayObject)) throw Exception("VertexArrayObject support is required.");
        if (!capabilities_.supports(Feature::Shader)) throw Exception("Shader support is required.");
        if (!capabilities_.supports(Feature::Program)) throw Exception("Program support is required.");
        if (!capabilities_.supports(Feature::Buffer)) throw Exception("Buffer support is required.");
        if (!capabilities_.supports(Feature::BasicRendering)) throw Exception("Basic rendering support is required.");

        // 5. Query Hardware Limits
        platform::GLint max_texture_size = 0;
        platform::g_gl.GetIntegerv(platform::GL_MAX_TEXTURE_SIZE, &max_texture_size);
        capabilities_.set_limit("max_texture_size", max_texture_size);

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
