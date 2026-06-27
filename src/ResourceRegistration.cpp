#include "easygl/ResourceRegistration.hpp"
#include "easygl/ResourceRegistry.hpp"
#include "easygl/RecoverableResource.hpp"

namespace easygl
{
    ResourceRegistration::ResourceRegistration(ResourceRegistry& registry, RecoverableResource& resource)
        : registry_(&registry)
        , resource_(&resource)
    {
        registry_->add(resource_);
    }

    ResourceRegistration::~ResourceRegistration()
    {
        registry_->remove(resource_);
    }
}
