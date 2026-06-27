#pragma once
#include <string_view>
#include "easygl/Export.hpp"

namespace easygl
{
    class Device;

    class EASYGL_API ScopedDebugGroup
    {
    public:
        ScopedDebugGroup(Device& device, std::string_view label, unsigned int id = 0);
        ~ScopedDebugGroup();

        ScopedDebugGroup(const ScopedDebugGroup&) = delete;
        ScopedDebugGroup& operator=(const ScopedDebugGroup&) = delete;
        ScopedDebugGroup(ScopedDebugGroup&&) = delete;
        ScopedDebugGroup& operator=(ScopedDebugGroup&&) = delete;

    private:
        Device* device_;
    };
}
