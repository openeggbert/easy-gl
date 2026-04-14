#include "easygl/Device.hpp"
#include "platform/GlFunctions.hpp"
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

        // Initialize internal GL function pointers
        if (!platform::initialize_gl_functions(platform::g_gl, loader_))
        {
            throw Exception("Failed to initialize GL function pointers.");
        }

        // TODO: Query real GL context information using glGetString/glGetStringi
        // For now, we use a stub for demonstration.
        ContextInfo info;
        info.api = ApiKind::OpenGL;
        info.major = 3;
        info.minor = 3;
        info.vendor = "easy-gl stub";
        info.renderer = "easy-gl stub renderer";
        info.version_string = "3.3.0 easy-gl stub";
        info.shading_language_version = "3.30";
        // info.extensions = ...; // TODO: Query extensions via glGetString(GL_EXTENSIONS) or glGetStringi

        capabilities_.set_context_info(std::move(info));
        
        // Basic feature detection based on version
        capabilities_.detect_common_features();

        // TODO: Query GL limits (e.g., GL_MAX_TEXTURE_SIZE) and set them
        // capabilities_.set_limit("max_texture_size", max_tex_size);

        initialized_ = true;
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
}
