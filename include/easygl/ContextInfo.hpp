#pragma once

#include <string>
#include <vector>
#include "easygl/Export.hpp"

namespace easygl
{
    enum class ApiKind
    {
        Unknown,
        OpenGL,
        OpenGLES
    };

    struct EASYGL_API ContextInfo
    {
        ApiKind api = ApiKind::Unknown;
        int major = 0;
        int minor = 0;
        std::string vendor;
        std::string renderer;
        std::string version_string;
        std::string shading_language_version;
        std::vector<std::string> extensions;

        [[nodiscard]] bool is_opengl() const noexcept { return api == ApiKind::OpenGL; }
        [[nodiscard]] bool is_opengles() const noexcept { return api == ApiKind::OpenGLES; }
    };
}