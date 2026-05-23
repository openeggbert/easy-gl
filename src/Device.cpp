#include "easygl/Device.hpp"
#include <metagl/metagl.hpp>
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

        // 1. Initialize meta-gl function pointers
        if (!metagl::Initialize(loader_))
        {
            throw Exception("Failed to bootstrap GL functions.");
        }

        // 2. Query Context Information
        const char* vendor_ptr    = metagl::glGetString(metagl::StringName::Vendor);
        const char* renderer_ptr  = metagl::glGetString(metagl::StringName::Renderer);
        const char* version_ptr   = metagl::glGetString(metagl::StringName::Version);
        const char* sl_version_ptr = metagl::glGetString(metagl::StringName::ShadingLanguageVersion);

        std::string vendor = vendor_ptr ? vendor_ptr : "";
        std::string renderer = renderer_ptr ? renderer_ptr : "";
        std::string version_string = version_ptr ? version_ptr : "";
        std::string shading_language_version = sl_version_ptr ? sl_version_ptr : "";

        ApiKind api = ApiKind::OpenGL;
        if (version_string.find("OpenGL ES") != std::string::npos)
        {
            api = ApiKind::OpenGLES;
        }

        metagl::GLint major = 0;
        metagl::GLint minor = 0;

        metagl::glGetIntegerv(metagl::IntegerName::MajorVersion, &major);
        metagl::glGetIntegerv(metagl::IntegerName::MinorVersion, &minor);

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
        if (major >= 3)
        {
            metagl::GLint num_extensions = 0;
            metagl::glGetIntegerv(metagl::IntegerName::NumExtensions, &num_extensions);
            for (int i = 0; i < num_extensions; ++i)
            {
                const char* ext = metagl::glGetStringi(metagl::StringName::Extensions, static_cast<metagl::GLuint>(i));
                if (ext) extensions.push_back(ext);
            }
        }
        else
        {
            const char* ext_ptr = metagl::glGetString(metagl::StringName::Extensions);
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
        metagl::GLint max_texture_size = 0;
        metagl::glGetIntegerv(metagl::IntegerName::MaxTextureSize, &max_texture_size);
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
        metagl::GLbitfield raw_mask = 0;
        if ((static_cast<u32>(flags) & static_cast<u32>(ClearFlags::Color)) != 0)
            raw_mask |= static_cast<metagl::GLbitfield>(metagl::ClearBufferBit::Color);
        if ((static_cast<u32>(flags) & static_cast<u32>(ClearFlags::Depth)) != 0)
            raw_mask |= static_cast<metagl::GLbitfield>(metagl::ClearBufferBit::Depth);
        if ((static_cast<u32>(flags) & static_cast<u32>(ClearFlags::Stencil)) != 0)
            raw_mask |= static_cast<metagl::GLbitfield>(metagl::ClearBufferBit::Stencil);

        metagl::glDisable(metagl::Capability::ScissorTest);
        metagl::glClear(static_cast<metagl::ClearBufferBit>(raw_mask));
    }

    void Device::set_clear_color(float r, float g, float b, float a)
    {
        metagl::glClearColor(r, g, b, a);
    }

    void Device::set_viewport(int x, int y, int width, int height)
    {
        metagl::glViewport(x, y, width, height);
    }

    void Device::get_viewport(int& x, int& y, int& width, int& height) const
    {
        metagl::GLint viewport[4];
        metagl::glGetIntegervRaw(0x0BA2 /* GL_VIEWPORT */, viewport);
        x = viewport[0];
        y = viewport[1];
        width = viewport[2];
        height = viewport[3];
    }

    void Device::set_blend_enabled(bool enabled)
    {
        if (enabled)
            metagl::glEnable(metagl::Capability::Blend);
        else
            metagl::glDisable(metagl::Capability::Blend);
    }

    void Device::set_blend_func(BlendFactor sfactor, BlendFactor dfactor)
    {
        metagl::glBlendFunc(sfactor, dfactor);
    }

    void Device::draw_arrays(PrimitiveType primitive, int first, int count)
    {
        metagl::glDrawArrays(primitive, first, count);
    }

    void Device::draw_elements(PrimitiveType primitive, int count, DataType type, const void* indices)
    {
        metagl::glDrawElements(primitive, count, type, indices);
    }

    void Device::set_depth_test_enabled(bool enabled)
    {
        if (enabled)
            metagl::glEnable(metagl::Capability::DepthTest);
        else
            metagl::glDisable(metagl::Capability::DepthTest);
    }

    void Device::set_depth_mask(bool enabled)
    {
        metagl::glDepthMask(enabled ? 1 : 0);
    }

    void Device::set_depth_func(CompareFunc func)
    {
        metagl::glDepthFunc(func);
    }

    void Device::set_clear_depth(float depth)
    {
        metagl::glClearDepthf(depth);
    }

    void Device::set_cull_face_enabled(bool enabled)
    {
        if (enabled)
            metagl::glEnable(metagl::Capability::CullFace);
        else
            metagl::glDisable(metagl::Capability::CullFace);
    }

    void Device::set_cull_face(CullFace face)
    {
        metagl::glCullFace(face);
    }

    void Device::set_front_face(FrontFace face)
    {
        metagl::glFrontFace(face);
    }
}
