#include "easygl/ScopedDebugGroup.hpp"
#include "easygl/Device.hpp"

namespace easygl
{
    ScopedDebugGroup::ScopedDebugGroup(Device& device, std::string_view label, unsigned int id)
        : device_(&device)
    {
        const std::string s(label);
        device_->push_debug_group(DebugSource::Application, id, s.c_str());
    }

    ScopedDebugGroup::~ScopedDebugGroup()
    {
        device_->pop_debug_group();
    }
}
