#include "easygl/Capabilities.hpp"
#include <algorithm>
#include <utility>

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
        // Basic version-based detection
        if (is_opengl())
        {
            set_feature(Feature::VertexArrayObject, is_at_least(3, 0));
            set_feature(Feature::FramebufferObject, is_at_least(3, 0));
            set_feature(Feature::UniformBufferObject, is_at_least(3, 1));
            set_feature(Feature::Texture3D, is_at_least(1, 2));
            set_feature(Feature::TextureFloat, is_at_least(3, 0));
            set_feature(Feature::TextureRG, is_at_least(3, 0));
            set_feature(Feature::Instancing, is_at_least(3, 1));
            set_feature(Feature::SamplerObject, is_at_least(3, 3));
            set_feature(Feature::GeometryShader, is_at_least(3, 2));
            set_feature(Feature::TessellationShader, is_at_least(4, 0));
            set_feature(Feature::ComputeShader, is_at_least(4, 3));
            set_feature(Feature::DirectStateAccess, is_at_least(4, 5));
            set_feature(Feature::DebugOutput, is_at_least(4, 3));

            // OpenGL Only
            set_feature(Feature::OpenGlOnlyImmediateMode, true);
            set_feature(Feature::OpenGlOnlyPolygonMode, true);
            set_feature(Feature::OpenGlOnlyLineWidthWide, true);
        }
        else if (is_opengles())
        {
            set_feature(Feature::VertexArrayObject, is_at_least(3, 0));
            set_feature(Feature::FramebufferObject, is_at_least(2, 0));
            set_feature(Feature::UniformBufferObject, is_at_least(3, 0));
            set_feature(Feature::Texture3D, is_at_least(3, 0));
            set_feature(Feature::TextureFloat, is_at_least(3, 0));
            set_feature(Feature::TextureRG, is_at_least(3, 0));
            set_feature(Feature::Instancing, is_at_least(3, 0));
            set_feature(Feature::SamplerObject, is_at_least(3, 0));
            set_feature(Feature::GeometryShader, is_at_least(3, 2));
            set_feature(Feature::TessellationShader, is_at_least(3, 2));
            set_feature(Feature::ComputeShader, is_at_least(3, 1));
            set_feature(Feature::DebugOutput, is_at_least(3, 2));

            // OpenGL Only (features not in GLES)
            set_feature(Feature::OpenGlOnlyImmediateMode, false);
            set_feature(Feature::OpenGlOnlyPolygonMode, false);
            set_feature(Feature::OpenGlOnlyLineWidthWide, false);
        }

        // Extension-based detection
        set_feature(Feature::AnisotropicFiltering, supports_extension("GL_EXT_texture_filter_anisotropic"));
        set_feature(Feature::TextureCompression, supports_extension("GL_EXT_texture_compression_s3tc") || 
                                               supports_extension("GL_KHR_texture_compression_astc_ldr"));
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

    bool Capabilities::is_at_least(int major, int minor) const noexcept
    {
        if (context_info_.major > major) return true;
        if (context_info_.major == major && context_info_.minor >= minor) return true;
        return false;
    }
}