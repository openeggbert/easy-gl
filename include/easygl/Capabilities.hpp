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

        [[nodiscard]] const ContextInfo& context_info() const noexcept;
        [[nodiscard]] bool supports(Feature feature) const;
        [[nodiscard]] bool supports_extension(const std::string& extension) const;
        [[nodiscard]] int get_limit(const std::string& name) const;
        [[nodiscard]] bool is_opengl() const noexcept;
        [[nodiscard]] bool is_opengles() const noexcept;

    private:
        ContextInfo context_info_{};
        std::unordered_map<Feature, bool> features_{};
        std::unordered_map<std::string, int> limits_{};
    };
}