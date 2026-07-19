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
        OpenGLES,
        /// Emscripten/WebGL context (only ever set when building with Emscripten).
        /// See @ref ContextInfo::is_webgl and Capabilities::is_webgl1/is_webgl2 for
        /// distinguishing WebGL 1 from WebGL 2, and easy-gl's TODO.md for the
        /// known permanent gaps this introduces (e.g. ProgramPipeline has no
        /// WebGL equivalent).
        WebGL
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
        [[nodiscard]] bool is_webgl() const noexcept { return api == ApiKind::WebGL; }
    };
}