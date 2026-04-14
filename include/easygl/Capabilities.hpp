#pragma once

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

        [[nodiscard]] const ContextInfo& context_info() const noexcept;
        [[nodiscard]] bool supports(Feature feature) const;
        [[nodiscard]] bool is_opengl() const noexcept;
        [[nodiscard]] bool is_opengles() const noexcept;

    private:
        ContextInfo context_info_{};
        std::unordered_map<Feature, bool> features_{};
    };
}