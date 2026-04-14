#pragma once

#include "easygl/Capabilities.hpp"
#include "easygl/Config.hpp"
#include "easygl/Exception.hpp"
#include "easygl/Export.hpp"
#include "easygl/Feature.hpp"
#include "easygl/Types.hpp"

namespace easygl
{
    class EASYGL_API Device
    {
    public:
        Device();
        explicit Device(Config config);

        void initialize(GLGetProcAddressFn loader);
        [[nodiscard]] bool is_initialized() const noexcept;

        [[nodiscard]] const Config& config() const noexcept;
        [[nodiscard]] const Capabilities& capabilities() const noexcept;

        [[nodiscard]] bool supports(Feature feature) const;
        void require(Feature feature) const;

    private:
        Config config_{};
        Capabilities capabilities_{};
        GLGetProcAddressFn loader_ = nullptr;
        bool initialized_ = false;
    };
}