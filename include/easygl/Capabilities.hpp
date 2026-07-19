#pragma once

#include <string>
#include <unordered_map>
#include "easygl/ContextInfo.hpp"
#include "easygl/Export.hpp"
#include "easygl/Feature.hpp"

namespace easygl
{
    class EASYGL_API Capabilities
    {
    public:
        Capabilities() = default;

        void set_context_info(ContextInfo info);
        void set_feature(Feature feature, bool supported);

        void set_limit(const std::string& name, int value);
        void detect_common_features();

        // Surfaced from metagl::Capabilities::webgl1/webgl2 by Device::initialize();
        // false on any non-Emscripten build. See is_webgl()/is_webgl1()/is_webgl2().
        void set_webgl(bool webgl1, bool webgl2) noexcept;

        [[nodiscard]] const ContextInfo& context_info() const noexcept;
        [[nodiscard]] bool supports(Feature feature) const;
        [[nodiscard]] bool supports_extension(const std::string& extension) const;
        [[nodiscard]] int get_limit(const std::string& name) const;
        [[nodiscard]] bool is_opengl() const noexcept;
        [[nodiscard]] bool is_opengles() const noexcept;
        [[nodiscard]] bool is_webgl() const noexcept;
        [[nodiscard]] bool is_webgl1() const noexcept;
        [[nodiscard]] bool is_webgl2() const noexcept;
        [[nodiscard]] bool is_at_least(int major, int minor) const noexcept;

    private:
        ContextInfo context_info_{};
        std::unordered_map<Feature, bool> features_{};
        std::unordered_map<std::string, int> limits_{};
        bool webgl1_ = false;
        bool webgl2_ = false;
    };
}