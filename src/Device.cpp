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

        // Initialize internal GL function pointers
        if (!platform::initialize_gl_functions(platform::g_gl, loader))
        {
            throw Exception("Failed to initialize GL function pointers.");
        }

        // TODO: Implement GL runtime loading and capability detection here.
        // For now, this is a minimal foundation stub.
        
        // Example of setting context info (stubs)
        ContextInfo info;
        info.api = ApiKind::OpenGL;
        info.major = 3;
        info.minor = 3;
        info.vendor = "easy-gl stub";
        info.renderer = "easy-gl stub renderer";
        info.version_string = "3.3.0 easy-gl stub";
        info.shading_language_version = "3.30";
        
        capabilities_.set_context_info(std::move(info));
        
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
