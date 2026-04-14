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
        // TODO: Replace this stub with real glGetString/glGetStringi calls
        // const char* vendor = reinterpret_cast<const char*>(platform::g_gl.GetString(0x1F00)); // GL_VENDOR
        // ...
        ContextInfo info;
        info.api = ApiKind::OpenGL;
        info.major = 3;
        info.minor = 3;
        info.vendor = "easy-gl stub";
        info.renderer = "easy-gl stub renderer";
        info.version_string = "3.3.0 easy-gl stub";
        info.shading_language_version = "3.30";
        // info.extensions = ...; // TODO: Query via glGetString(GL_EXTENSIONS) or glGetStringi

        capabilities_.set_context_info(std::move(info));
        
        // 3. Detect features based on version and extensions
        capabilities_.detect_common_features();

        // 4. Query Hardware Limits
        // TODO: Query GL limits (e.g., GL_MAX_TEXTURE_SIZE) using glGetIntegerv
        // GLint max_tex_size = 0;
        // platform::g_gl.GetIntegerv(0x0D33, &max_tex_size); // GL_MAX_TEXTURE_SIZE
        // capabilities_.set_limit("max_texture_size", max_tex_size);

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
}
