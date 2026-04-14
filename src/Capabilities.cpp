#include "easygl/Capabilities.hpp"

namespace easygl
{
    void Capabilities::set_context_info(ContextInfo info)
    {
        context_info_ = std::move(info);
    }

    void Capabilities::set_feature(Feature feature, bool supported)
    {
        features_[feature] = supported;
    }

    const ContextInfo& Capabilities::context_info() const noexcept
    {
        return context_info_;
    }

    bool Capabilities::supports(Feature feature) const
    {
        const auto it = features_.find(feature);
        if (it == features_.end())
        {
            return false;
        }
        return it->second;
    }

    bool Capabilities::is_opengl() const noexcept
    {
        return context_info_.is_opengl();
    }

    bool Capabilities::is_opengles() const noexcept
    {
        return context_info_.is_opengles();
    }
}