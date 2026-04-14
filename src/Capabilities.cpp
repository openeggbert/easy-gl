#include "easygl/Capabilities.hpp"
#include <algorithm>

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

    void Capabilities::set_limit(const std::string& name, int value)
    {
        limits_[name] = value;
    }

    void Capabilities::detect_common_features()
    {
        const int major = context_info_.major;
        const int minor = context_info_.minor;
        const bool is_gl = context_info_.is_opengl();
        const bool is_gles = context_info_.is_opengles();

        // Basic version-based detection
        if (is_gl)
        {
            set_feature(Feature::VertexArrayObject, major >= 3);
            set_feature(Feature::FramebufferObject, major >= 3);
            set_feature(Feature::UniformBufferObject, (major > 3) || (major == 3 && minor >= 1));
            set_feature(Feature::Texture3D, (major > 1) || (major == 1 && minor >= 2));
            set_feature(Feature::TextureFloat, major >= 3);
            set_feature(Feature::TextureRG, major >= 3);
            set_feature(Feature::Instancing, (major > 3) || (major == 3 && minor >= 1));
            set_feature(Feature::SamplerObject, (major > 3) || (major == 3 && minor >= 3));
            set_feature(Feature::GeometryShader, (major > 3) || (major == 3 && minor >= 2));
            set_feature(Feature::TessellationShader, major >= 4);
            set_feature(Feature::ComputeShader, (major > 4) || (major == 4 && minor >= 3));
            set_feature(Feature::DirectStateAccess, (major > 4) || (major == 4 && minor >= 5));
            set_feature(Feature::DebugOutput, (major > 4) || (major == 4 && minor >= 3));

            // OpenGL Only
            set_feature(Feature::OpenGlOnlyImmediateMode, true);
            set_feature(Feature::OpenGlOnlyPolygonMode, true);
            set_feature(Feature::OpenGlOnlyLineWidthWide, true);
        }
        else if (is_gles)
        {
            set_feature(Feature::VertexArrayObject, major >= 3);
            set_feature(Feature::FramebufferObject, major >= 2);
            set_feature(Feature::UniformBufferObject, major >= 3);
            set_feature(Feature::Texture3D, major >= 3);
            set_feature(Feature::TextureFloat, major >= 3);
            set_feature(Feature::TextureRG, major >= 3);
            set_feature(Feature::Instancing, major >= 3);
            set_feature(Feature::SamplerObject, major >= 3);
            set_feature(Feature::GeometryShader, (major > 3) || (major == 3 && minor >= 2));
            set_feature(Feature::TessellationShader, (major > 3) || (major == 3 && minor >= 2));
            set_feature(Feature::ComputeShader, (major > 3) || (major == 3 && minor >= 1));
            set_feature(Feature::DebugOutput, (major > 3) || (major == 3 && minor >= 2));

            // OpenGL Only
            set_feature(Feature::OpenGlOnlyImmediateMode, false);
            set_feature(Feature::OpenGlOnlyPolygonMode, false);
            set_feature(Feature::OpenGlOnlyLineWidthWide, false);
        }

        // Texture compression and anisotropic filtering are usually extension-based
        // or available in newer versions.
        // For now, these remain as placeholders or can be checked against extensions.
        set_feature(Feature::AnisotropicFiltering, supports_extension("GL_EXT_texture_filter_anisotropic"));
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

    bool Capabilities::supports_extension(const std::string& extension) const
    {
        return std::find(context_info_.extensions.begin(), context_info_.extensions.end(), extension) 
               != context_info_.extensions.end();
    }

    int Capabilities::get_limit(const std::string& name) const
    {
        const auto it = limits_.find(name);
        if (it == limits_.end())
        {
            return 0;
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