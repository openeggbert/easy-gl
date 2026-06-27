#pragma once
#include "easygl/Export.hpp"

namespace easygl
{
    class ResourceRegistry;
    class RecoverableResource;

    class EASYGL_API ResourceRegistration
    {
    public:
        ResourceRegistration(ResourceRegistry& registry, RecoverableResource& resource);
        ~ResourceRegistration();

        ResourceRegistration(const ResourceRegistration&) = delete;
        ResourceRegistration& operator=(const ResourceRegistration&) = delete;
        ResourceRegistration(ResourceRegistration&&) = delete;
        ResourceRegistration& operator=(ResourceRegistration&&) = delete;

    private:
        ResourceRegistry* registry_;
        RecoverableResource* resource_;
    };
}
