#include "easygl/ResourceRegistry.hpp"
#include <metagl/metagl.hpp>
#include <algorithm>

namespace easygl
{
    ResourceRegistry::~ResourceRegistry()
    {
        unregister_from_meta_gl();
    }

    void ResourceRegistry::add(RecoverableResource* resource)
    {
        if (!resource) return;
        resources_.push_back(resource);
    }

    void ResourceRegistry::remove(RecoverableResource* resource)
    {
        auto& v = resources_;
        v.erase(std::remove(v.begin(), v.end(), resource), v.end());
    }

    void ResourceRegistry::register_with_meta_gl()
    {
        if (!registered_with_meta_gl_)
        {
            metagl::AddContextListener(this);
            registered_with_meta_gl_ = true;
        }
    }

    void ResourceRegistry::unregister_from_meta_gl()
    {
        if (registered_with_meta_gl_)
        {
            metagl::RemoveContextListener(this);
            registered_with_meta_gl_ = false;
        }
    }

    void ResourceRegistry::OnContextLost()
    {
        // Context is lost: do not call gl* functions.
        // Release GPU handles without issuing any GL calls.
        for (auto* r : resources_)
            if (r) r->release_gl_handle_only();
    }

    void ResourceRegistry::OnContextRestored()
    {
        // Context has been restored and function pointers reloaded.
        // Recreate all GPU resources.
        for (auto* r : resources_)
            if (r) r->recreate_gl_resource();
    }
}
